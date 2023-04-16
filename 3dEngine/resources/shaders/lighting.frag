#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const uint MAX_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 1) const uint MAX_SHADOW_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 2) const uint NUMBER_SHADOW_MAPS = 7; //must be equals to ShadowManager::SHADOW_MAPS_SHADER_LIMIT
layout(constant_id = 3) const float SHADOW_MAP_CONSTANT_BIAS = 0.0;
layout(constant_id = 4) const float SHADOW_MAP_SLOPE_BIAS_FACTOR = 0.0;
layout(constant_id = 5) const float MAX_EMISSIVE_FACTOR = 0.0;

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
    vec3 color;
} fog;

//deferred textures
layout(binding = 6) uniform sampler2D depthTex; //depth (32 bits)
layout(binding = 7) uniform sampler2D albedoAndEmissiveTex; //albedo RGB (3 * 8 bits) + emissive factor (8 bits)
layout(binding = 8) uniform sampler2D normalAndAmbientTex; //normal XYZ (3 * 8 bits) + ambient factor (8 bits)
layout(binding = 9) uniform sampler2D materialTex; //roughness (8 bits) + metalness (8 bits)
layout(binding = 10) uniform sampler2D ambientOcclusionTex; //ambient occlusion (8 or 16 bits)
layout(binding = 11) uniform sampler2D transparencyAccumulationTex; //transparency accumulation (4 * 16 bits)
layout(binding = 12) uniform sampler2D transparencyRevealTex; //transparency reveal (1 * 8 bits)
layout(binding = 13) uniform sampler2DArray shadowMapTex[MAX_SHADOW_LIGHTS]; //shadow maps for each lights (2 * 32 bits * nbSplit * nbLight)

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
    float slopeBias = (1.0 - NdotL) * SHADOW_MAP_SLOPE_BIAS_FACTOR;
    float bias = SHADOW_MAP_CONSTANT_BIAS + slopeBias;
    float shadowMapZBias = shadowMapZ - bias;
    if (shadowMapZBias <= moments.x) { //is in hard/full shadow
        return 1.0; //no attentuation / no shadow
    }

    float variance = moments.y - (moments.x * moments.x);
    float d = shadowMapZBias - moments.x;
    float pMax = variance / (variance + d * d);
    pMax = linearStep(0.75, 1.0, pMax); //reduce light bleeding where first parameter is at the graphic designer appreciation
    return max(pMax, NdotL / 10.0); //hijack to apply normal map in shadow
}

float computeShadowAttenuation(int shadowLightIndex, float depthValue, vec4 position, float NdotL) {
    float shadowAttenuation = 1.0; //1.0 = no shadow

    for (int i = 0; i < NUMBER_SHADOW_MAPS; ++i) {
        if (depthValue < shadowMap.depthSplitDistance[i]) {
            vec4 shadowCoord = shadowLight.mLightProjectionView[shadowLightIndex * MAX_SHADOW_LIGHTS + i] * position;

            //model has produceShadow flag to true ?
            if (shadowCoord.s <= 1.0 && shadowCoord.s >= -1.0 && shadowCoord.t <= 1.0 && shadowCoord.t >= -1.0) {
                shadowCoord.s = (shadowCoord.s / 2.0) + 0.5;
                shadowCoord.t = (shadowCoord.t / 2.0) + 0.5;

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

vec3 addFog(vec3 baseColor, vec4 position) {
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

vec3 addTransparentModels(vec3 srcAlbedo) {
    float reveal = texture(transparencyRevealTex, texCoordinates).r; //(1 - obj1.material.alpha) * (1 - obj2.material.alpha) * ...
    if (reveal > 0.99999) {
        //fully transparent case: object fully transparent or no object
        return srcAlbedo;
    }

    vec4 accumulation = texture(transparencyAccumulationTex, texCoordinates); //(obj1.material.rgb * obj1.material.a, obj1.material.a) * weight1 + ...
    if (isinf(maxComponent(abs(accumulation.rgb)))) { //suppress overflow
        accumulation.rgb = vec3(accumulation.a);
    }

    vec4 averageColor = vec4(vec3(accumulation.rgb / max(accumulation.a, 0.00001)), 1.0 - reveal);

    //apply blending manually (equivalent to: srcFactor=SRC_ALPHA, dstFactor=ONE_MINUS_SRC_ALPHA)
    return averageColor.a * averageColor.rgb + (1 - averageColor.a) * srcAlbedo.rgb;
}

float distributionGGX(vec3 normal, vec3 halfWay, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH  = max(dot(normal, halfWay), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (alpha2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;
    return alpha2 / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float alpha = roughness * roughness;
    float k = alpha / 2.0f;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 normal, vec3 vertexToCameraPos, vec3 vertexToLight, float roughness) {
    float NdotV = max(dot(normal, vertexToCameraPos), 0.0);
    float NdotL = max(dot(normal, vertexToLight), 0.0);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(vec3 halfWay, vec3 vertexToCameraPos, vec3 baseReflectivity) {
    //See https://en.wikipedia.org/wiki/Schlick%27s_approximation
    float VdotH = max(dot(halfWay, vertexToCameraPos), 0.0);
    return baseReflectivity + (1.0 - baseReflectivity) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

void main() {
    float depthValue = texture(depthTex, texCoordinates).r;
    vec4 albedoAndEmissive = texture(albedoAndEmissiveTex, texCoordinates);
    vec4 normalAndAmbient = texture(normalAndAmbientTex, texCoordinates);

    vec4 worldPosition = fetchWorldPosition(texCoordinates, depthValue);
    vec3 albedo = albedoAndEmissive.rgb;
    float emissiveFactor = albedoAndEmissive.a * MAX_EMISSIVE_FACTOR; //unpack emissive factor
    float modelAmbientFactor = normalAndAmbient.a;

    if (modelAmbientFactor < 0.9999) { //apply lighting
        vec3 vertexToCameraPos = normalize(positioningData.viewPosition - vec3(worldPosition));
        vec3 normal = normalize(vec3(normalAndAmbient) * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
        vec3 modelAmbient = albedo * modelAmbientFactor;

        fragColor = vec4(lightsData.globalAmbient, 1.0); //start with global ambient

        if (visualOption.hasAmbientOcclusion) {
            float ambientOcclusionFactor = texture(ambientOcclusionTex, texCoordinates).r;
            fragColor.rgb -= vec3(ambientOcclusionFactor, ambientOcclusionFactor, ambientOcclusionFactor); //subtract ambient occlusion
        }

        const vec3 dielectricSurfacesBaseReflectivity = vec3(0.04); //value is a mean of all no-metallic surfaces (plastic, water, ruby, diamond, glass...)
        vec2 materialValues = texture(materialTex, texCoordinates).rg;
        float roughness = materialValues.r;
        float metallic = materialValues.g;
        vec3 baseReflectivity = mix(dielectricSurfacesBaseReflectivity, albedo, metallic);

        fragColor.rgb += albedo * emissiveFactor; //add emissive lighting

        for (int lightIndex = 0, shadowLightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
            if (!lightsData.lightsInfo[lightIndex].isExist) {
                break;//no more light
            }

            LightValues lightValues = computeLightValues(lightsData.lightsInfo[lightIndex], normal, vec3(worldPosition));

            vec3 lightRadiance = lightsData.lightsInfo[lightIndex].lightColor * lightValues.lightAttenuation;
            vec3 bidirectionalReflectanceDist;
            if ((lightsData.lightsInfo[lightIndex].lightFlags & LIGHT_FLAG_PBR_ENABLED) != 0) {
                //PBR formulas (see https://www.youtube.com/watch?v=RRE-F57fbXw & https://learnopengl.com/PBR/Theory)
                vec3 halfWay = normalize(vertexToCameraPos + lightValues.vertexToLight);
                float normalDistribution = distributionGGX(normal, halfWay, roughness);
                float geometryShadowing = geometrySmith(normal, vertexToCameraPos, lightValues.vertexToLight, roughness);
                vec3 fresnelFactor = fresnelSchlick(halfWay, vertexToCameraPos, baseReflectivity);
                vec3 kS = fresnelFactor;
                vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
                vec3 cookTorranceSpecular = (normalDistribution * geometryShadowing * fresnelFactor) / (4.0 * max(dot(normal, vertexToCameraPos), 0.0) * lightValues.NdotL + 0.0001);
                vec3 lambert = albedo;//do not divide by PI (see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/)
                bidirectionalReflectanceDist = kD * lambert + cookTorranceSpecular;
            } else {
                bidirectionalReflectanceDist = albedo;
            }

            //shadow
            float shadowAttenuation = 1.0; //1.0 = no shadow
            if (visualOption.hasShadow && (lightsData.lightsInfo[lightIndex].lightFlags & LIGHT_FLAG_PRODUCE_SHADOW) != 0) {
                shadowAttenuation = computeShadowAttenuation(shadowLightIndex, depthValue, worldPosition, lightValues.NdotL);
                shadowLightIndex++;
            }

            fragColor.rgb += modelAmbient * lightValues.lightAttenuation; //add ambient
            fragColor.rgb += shadowAttenuation * (bidirectionalReflectanceDist * lightRadiance * lightValues.NdotL); //update with PBR formula
        }
    } else { //do not apply lighting (e.g. skybox, geometry models...)
        fragColor.rgb = albedo * (1.0 + emissiveFactor); //albedo + add emissive lighting
    }

    fragColor.rgb = addTransparentModels(fragColor.rgb);
    fragColor.rgb = addFog(fragColor.rgb, worldPosition);

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
