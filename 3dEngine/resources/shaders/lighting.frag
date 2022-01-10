#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const uint MAX_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 1) const uint MAX_SHADOW_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 2) const uint NUMBER_SHADOW_MAPS = 7; //must be equals to ShadowManager::SHADOW_MAPS_SHADER_LIMIT
layout(constant_id = 3) const float SHADOW_MAP_BIAS = 0.0;
layout(constant_id = 4) const float MAX_EMISSIVE_FACTOR = 0.0;

//global
layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseViewProjection;
    vec3 viewPosition;
} positioningData;
layout(std140, set = 0, binding = 1) uniform VisualOption {
    bool hasShadow;
    bool hasAmbientOcclusion;
} visualOption;

//lighting
layout(std140, set = 0, binding = 2) uniform LightsData {
    LightInfo lightsInfo[MAX_LIGHTS];
    vec3 globalAmbient;
} lightsData;

//shadow
layout(std140, set = 0, binding = 3) uniform ShadowLight {
    mat4 mLightProjectionView[MAX_SHADOW_LIGHTS * NUMBER_SHADOW_MAPS]; //use 1 dim. table because 2 dim. tables are bugged (only in RenderDoc ?)
} shadowLight;
layout(std140, set = 0, binding = 4) uniform ShadowMap {
    float depthSplitDistance[NUMBER_SHADOW_MAPS];
} shadowMap;

//fog
layout(std140, set = 0, binding = 5) uniform Fog {
    bool hasFog;
    float density;
    float gradient;
    float maxHeight;
    vec4 color;
} fog;

//deferred textures
layout(binding = 6) uniform sampler2D depthTex; //depth (32 bits)
layout(binding = 7) uniform sampler2D colorAndEmissiveTex; //diffuse RGB (3 * 8 bits) + emissive factor
layout(binding = 8) uniform sampler2D normalAndAmbientTex; //normal XYZ (3 * 8 bits) + ambient factor
layout(binding = 9) uniform sampler2D ambientOcclusionTex; //ambient occlusion (8 or 16 bits)
layout(binding = 10) uniform sampler2D transparencyAccumulationTex; //transparency accumulation (4 * 16 bits)
layout(binding = 11) uniform sampler2D transparencyRevealTex; //transparency reveal (1 * 8 bits)
layout(binding = 12) uniform sampler2DArray shadowMapTex[MAX_SHADOW_LIGHTS]; //shadow maps for each lights (2 * 32 bits * nbSplit * nbLight)

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec4 fetchWorldPosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0 - 1.0,
        texCoord.t * 2.0 - 1.0,
        depthValue,
        1.0
    );
    vec4 worldPosition = positioningData.mInverseViewProjection * texPosition;
    worldPosition /= worldPosition.w;
    return worldPosition;
}

float linearStep(float min, float max, float v) {
      return clamp((v - min) / (max - min), 0.0, 1.0);
}

float maxComponent(vec3 components) {
    return max(max(components.x, components.y), components.z);
}

float computeShadowAttenuation(float shadowMapZ, vec2 moments, float NdotL) {
    float tanAcosNdotL = sqrt(1.0 - NdotL * NdotL) / NdotL; //=tan(acos(NdotL))
    float bias = max(SHADOW_MAP_BIAS * tanAcosNdotL, 0.00001);
    float shadowMapZBias = shadowMapZ - bias;
    if (shadowMapZBias <= moments.x) {
        return 1.0; //no attentuation / no shadow
    }

    float variance = moments.y - (moments.x * moments.x);
    float d = shadowMapZBias - moments.x;
    float pMax = variance / (variance + d * d);

    pMax = linearStep(0.75, 1.0, pMax); //reduce light bleeding where first parameter is at the graphic designer appreciation
    return max(pMax, NdotL / 15.0f); //hijack to apply normal map in shadow
}

float computeShadowAttenuation(int shadowLightIndex, float depthValue, vec4 position, float NdotL) {
    float shadowAttenuation = 1.0; //1.0 = no shadow

    for (int i = 0; i < NUMBER_SHADOW_MAPS; ++i) {
        if (depthValue < shadowMap.depthSplitDistance[i]) {
            vec4 shadowCoord = shadowLight.mLightProjectionView[shadowLightIndex * MAX_SHADOW_LIGHTS + i] * position;
            shadowCoord.s = (shadowCoord.s / 2.0) + 0.5;
            shadowCoord.t = (shadowCoord.t / 2.0) + 0.5;

            //model has produceShadow flag to true ?
            if (shadowCoord.s <= 1.0 && shadowCoord.s >= 0.0 && shadowCoord.t <= 1.0 && shadowCoord.t >= 0.0) {
                vec2 moments = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st, i)).rg;
                shadowAttenuation = computeShadowAttenuation(shadowCoord.z, moments, NdotL);

                //DEBUG: shadow without variance shadow map feature:
                /*shadowAttenuation = 1.0;
                float sDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st, i)).r;
                if (shadowCoord.z - 0.001 > sDepth) {
                    shadowAttenuation = 0.0;
                } */
            }

            break;
        }
    }

    return shadowAttenuation;
}

vec4 addFog(vec4 baseColor, vec4 position) {
    if (!fog.hasFog || positioningData.viewPosition.y > fog.maxHeight) {
        return baseColor;
    }

    vec3 lineVector = position.xyz - positioningData.viewPosition;
    float t = (fog.maxHeight - positioningData.viewPosition.y) / lineVector.y;
    vec3 correctedPosition = position.xyz;
    if (t > 0.0 && t < 1.0) {
        correctedPosition = positioningData.viewPosition + (t * lineVector);
    }

    float distance = distance(positioningData.viewPosition, correctedPosition);
    float visibility = exp(-pow((distance * fog.density), fog.gradient));
    return mix(fog.color, baseColor, visibility);
}

vec4 addTransparentModels(vec4 srcDiffuse) {
    float reveal = texture(transparencyRevealTex, texCoordinates).r; //(1 - obj1.material.alpha) * (1 - obj2.material.alpha) * ...
    if (reveal > 0.99999) {
        //fully transparent case: object fully transparent or no object
        return srcDiffuse;
    }

    vec4 accumulation = texture(transparencyAccumulationTex, texCoordinates); //(obj1.material.rgb * obj1.material.a, obj1.material.a) * weight1 + ...
    if (isinf(maxComponent(abs(accumulation.rgb)))) { //suppress overflow
        accumulation.rgb = vec3(accumulation.a);
    }

    vec4 averageColor = vec4(vec3(accumulation.rgb / max(accumulation.a, 0.00001)), 1.0 - reveal);

    //apply blending manually (equivalent to: srcFactor=SRC_ALPHA, dstFactor=ONE_MINUS_SRC_ALPHA)
    return averageColor.a * averageColor.rgba + (1 - averageColor.a) * srcDiffuse.rgba;
}

void main() {
    float depthValue = texture(depthTex, texCoordinates).r;
    vec4 diffuseAndEmissive = texture(colorAndEmissiveTex, texCoordinates);
    vec4 normalAndAmbient = texture(normalAndAmbientTex, texCoordinates);

    vec4 worldPosition = fetchWorldPosition(texCoordinates, depthValue);
    vec3 diffuse = diffuseAndEmissive.rgb;
    float emissiveFactor = diffuseAndEmissive.a * MAX_EMISSIVE_FACTOR; //unpack emissive factor
    float modelAmbientFactor = normalAndAmbient.a;

    if (modelAmbientFactor < 0.9999) { //apply lighting
        vec3 normal = vec3(normalAndAmbient) * 2.0 - 1.0;
        vec3 modelAmbient = diffuse * modelAmbientFactor;
        fragColor = vec4(lightsData.globalAmbient, 1.0);

        if (visualOption.hasAmbientOcclusion) {
            float ambientOcclusionFactor = texture(ambientOcclusionTex, texCoordinates).r;
            fragColor.rgb -= ambientOcclusionFactor, ambientOcclusionFactor, ambientOcclusionFactor;
        }

        float emissiveAttenuation = max(0.0, 1.0 - emissiveFactor); //disable lighting on highly emissive objects (give better results)
        for (int lightIndex = 0, shadowLightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
            if (lightsData.lightsInfo[lightIndex].isExist) {
                float NdotL = 0.0;
                float lightAttenuation = computeLightAttenuation(lightsData.lightsInfo[lightIndex], normal, vec3(worldPosition), NdotL);
                vec3 ambient = lightsData.lightsInfo[lightIndex].lightAmbient * modelAmbient;

                float shadowAttenuation = 1.0; //1.0 = no shadow
                if (visualOption.hasShadow && lightsData.lightsInfo[lightIndex].produceShadow) {
                    shadowAttenuation = computeShadowAttenuation(shadowLightIndex, depthValue, worldPosition, NdotL);
                    shadowLightIndex++;
                }

                fragColor.rgb += emissiveAttenuation * lightAttenuation * (shadowAttenuation * (diffuse * NdotL) + ambient);
            } else {
                break; //no more light
            }
        }
        fragColor.rgb += diffuse * emissiveFactor;
    } else { //do not apply lighting (e.g. skybox, geometry models...)
        fragColor.rgb = diffuse;
    }

    fragColor = addTransparentModels(fragColor);
    fragColor = addFog(fragColor, worldPosition);

    //DEBUG: add color to shadow map splits
    /* const float colorValue = 0.25;
    vec4 splitColors[5] = vec4[](
        vec4(colorValue, 0.0, 0.0, 1.0), vec4(0.0, colorValue, 0.0, 1.0), vec4(0.0, 0.0, colorValue, 1.0),
        vec4(colorValue, 0.0, colorValue, 1.0), vec4(colorValue, colorValue, 0.0, 1.0));
    for (int i = 0; i < NUMBER_SHADOW_MAPS; ++i) {
        if (depthValue < shadowMap.depthSplitDistance[i]) {
            fragColor += splitColors[i % 5];
            break;
        }
    } */
}
