#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const uint MAX_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 1) const float AO_STRENGTH = 0.0;
layout(constant_id = 2) const uint MAX_SHADOW_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 3) const uint MAX_SPLIT_SHADOW_MAPS = 6; //must be equals to ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT
layout(constant_id = 4) const float SHADOW_MAP_CONSTANT_BIAS_FACTOR = 0.0;
layout(constant_id = 5) const float SHADOW_MAP_SLOPE_BIAS_FACTOR = 0.0;
layout(constant_id = 6) const int SHADOW_MAP_OFFSET_TEX_SIZE = 0;
layout(constant_id = 7) const float MAX_EMISSIVE_FACTOR = 0.0;

//global
layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseProjectionView;
    vec3 viewPosition;
} positioningData;
layout(std140, set = 0, binding = 1) uniform SceneInfo {
    bool hasShadow;
    bool hasAmbientOcclusion;
} sceneInfo;

//lighting
layout(std140, set = 0, binding = 2) uniform LightsData {
    LightInfo lightsInfo[MAX_LIGHTS];
    vec3 globalAmbient;
} lightsData;

//shadow
layout(std140, set = 0, binding = 3) uniform ShadowLight {
    mat4 mLightProjectionView[MAX_SHADOW_LIGHTS * MAX_SPLIT_SHADOW_MAPS]; //use 1 dim. table because 2 dim. tables are bugged (only in RenderDoc ?)
} shadowLight;
layout(std140, set = 0, binding = 4) uniform ShadowMapData {
    vec4 splitData[MAX_SPLIT_SHADOW_MAPS];
} shadowMapData;
layout(std140, set = 0, binding = 5) uniform ShadowMapInfo {
    int offsetSampleCount;
} shadowMapInfo;

//fog
layout(std140, set = 0, binding = 6) uniform Fog {
    bool hasFog;
    float density;
    float gradient;
    float maxHeight;
    vec3 color;
} fog;

//deferred textures
layout(binding = 7) uniform sampler2D depthTex; //depth (32 bits)
layout(binding = 8) uniform sampler2D albedoAndEmissiveTex; //albedo RGB (3 * 8 bits) + emissive factor (8 bits)
layout(binding = 9) uniform sampler2D normalAndAmbientTex; //normal XYZ (3 * 8 bits) + ambient factor (8 bits)
layout(binding = 10) uniform usampler2D materialAndMaskTex; //roughness (8 bits) + metalness (8 bits) + light mask (8 bits)
layout(binding = 11) uniform sampler2D ambientOcclusionTex; //ambient occlusion (8 or 16 bits)
layout(binding = 12) uniform sampler2DArray shadowMapTex[MAX_SHADOW_LIGHTS]; //shadow maps for each lights (32 bits * nbSplit * nbLight)
layout(binding = 13) uniform sampler2DArray shadowMapOffsetTex; //shadow maps offset (32 bits)

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec4 fetchWorldPosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0 - 1.0,
        texCoord.t * 2.0 - 1.0,
        depthValue,
        1.0
    );
    vec4 worldPosition = positioningData.mInverseProjectionView * texPosition;
    worldPosition /= worldPosition.w;
    return worldPosition;
}

float linearStep(float min, float max, float v) {
      return clamp((v - min) / (max - min), 0.0, 1.0);
}

float maxComponent(vec3 components) {
    return max(max(components.x, components.y), components.z);
}

float computeShadowQuantity(int shadowLightIndex, int splitShadowMapIndex, vec4 worldPosition, float NdotL, float biasReduceFactor) {
    float shadowQuantity = 0.0f;

    vec4 shadowCoord = shadowLight.mLightProjectionView[shadowLightIndex * MAX_SPLIT_SHADOW_MAPS + splitShadowMapIndex] * worldPosition;
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.s = (shadowCoord.s / 2.0) + 0.5;
    shadowCoord.t = (shadowCoord.t / 2.0) + 0.5;

    float slopeBias = (1.0 - NdotL) * SHADOW_MAP_SLOPE_BIAS_FACTOR;
    float bias = (slopeBias + SHADOW_MAP_CONSTANT_BIAS_FACTOR) * biasReduceFactor;

    const float SOFT_EDGE_LENGTH = 1.5f;
    float shadowMapInvSize = 1.0 / float(textureSize(shadowMapTex[shadowLightIndex], 0));
    int testPointsQuantity = min(5, shadowMapInfo.offsetSampleCount);
    vec2 sceneSize = textureSize(depthTex, 0);
    ivec2 offsetTexCoordinate = ivec2(texCoordinates * sceneSize) % ivec2(SHADOW_MAP_OFFSET_TEX_SIZE, SHADOW_MAP_OFFSET_TEX_SIZE);

    int testPointsInShadow = 0;
    int offsetSampleIndex = 0;
    for (; offsetSampleIndex < testPointsQuantity; ++offsetSampleIndex) {
        vec2 shadowMapOffset = texelFetch(shadowMapOffsetTex, ivec3(offsetTexCoordinate, offsetSampleIndex), 0).xy * SOFT_EDGE_LENGTH * shadowMapInvSize;
        float shadowDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st + shadowMapOffset, splitShadowMapIndex)).r;
        float adjustedBias = bias * (1.0 + dot(shadowMapOffset, shadowMapOffset));
        if (shadowCoord.z - adjustedBias > shadowDepth) {
            shadowQuantity += 1.0f;
            testPointsInShadow++;
        }
    }

    if (testPointsInShadow != 0 && testPointsInShadow != testPointsQuantity) {
        for (; offsetSampleIndex < shadowMapInfo.offsetSampleCount; ++offsetSampleIndex) {
            vec2 shadowMapOffset = texelFetch(shadowMapOffsetTex, ivec3(offsetTexCoordinate, offsetSampleIndex), 0).xy * SOFT_EDGE_LENGTH * shadowMapInvSize;
            float shadowDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st + shadowMapOffset, splitShadowMapIndex)).r;
            float adjustedBias = bias * (1.0 + dot(shadowMapOffset, shadowMapOffset));
            if (shadowCoord.z - adjustedBias > shadowDepth) {
                shadowQuantity += 1.0f;
            }
        }
    }
    shadowQuantity /= offsetSampleIndex;

    //DEBUG: fetch shadow map one time, no PCF filter
    /* shadowQuantity = 0.0f;
    float shadowDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st, splitShadowMapIndex)).r;
    if (shadowCoord.z - bias > shadowDepth) {
        shadowQuantity = 1.0f;
    } */

    return shadowQuantity;
}

float computeSunShadowQuantity(int shadowLightIndex, vec4 worldPosition, float NdotL) {
    for (int splitShadowMapIndex = 0; splitShadowMapIndex < MAX_SPLIT_SHADOW_MAPS; ++splitShadowMapIndex) {
        float frustumCenterDist = distance(vec3(worldPosition), shadowMapData.splitData[splitShadowMapIndex].xyz);
        float frustumRadius = shadowMapData.splitData[splitShadowMapIndex].w;
        if (frustumCenterDist < frustumRadius) {
            return computeShadowQuantity(shadowLightIndex, splitShadowMapIndex, worldPosition, NdotL, 1.0f);
        }
    }
    return 0.0;
}

float computeOmnidirectionalShadowQuantity(int shadowLightIndex, vec4 worldPosition, float NdotL, vec3 lightPosition) {
    vec3 lightToFragment = vec3(worldPosition) - lightPosition;
    vec3 absDir = abs(lightToFragment);

    int shadowMapIndex = 0;
    if (absDir.x > absDir.y && absDir.x > absDir.z) {
        shadowMapIndex = lightToFragment.x > 0.0 ? 0 /* Right (X+) */ : 1 /* Left (X-) */;
    } else if (absDir.y > absDir.x && absDir.y > absDir.z) {
        shadowMapIndex = lightToFragment.y > 0.0 ? 2 /* Top (Y+) */ : 3 /* Bottom (Y-) */;
    } else {
        shadowMapIndex = lightToFragment.z > 0.0 ? 4 /* Front (Z+) */ : 5 /* Back (Z-) */;
    }

    float biasReduceFactor = 0.15f; //specific bias because shadow map depth is not linear
    return computeShadowQuantity(shadowLightIndex, shadowMapIndex, worldPosition, NdotL, biasReduceFactor);
}

float computeSpotShadowQuantity(int shadowLightIndex, vec4 worldPosition, float NdotL) {
    float biasReduceFactor = 0.15f; //specific bias because shadow map depth is not linear
    return computeShadowQuantity(shadowLightIndex, 0, worldPosition, NdotL, biasReduceFactor);
}

vec3 addFog(vec3 baseColor, vec4 worldPosition) {
    if (!fog.hasFog || positioningData.viewPosition.y > fog.maxHeight) {
        return baseColor;
    }

    vec3 lineVector = worldPosition.xyz - positioningData.viewPosition;
    float t = (fog.maxHeight - positioningData.viewPosition.y) / lineVector.y;
    vec3 correctedPosition = worldPosition.xyz;
    if (t > 0.0 && t < 1.0) {
        correctedPosition = positioningData.viewPosition + (t * lineVector);
    }

    float distance = distance(positioningData.viewPosition, correctedPosition);
    float visibility = exp(-pow((distance * fog.density), fog.gradient));
    return mix(fog.color, baseColor, visibility);
}

float distributionGGX(vec3 normal, vec3 halfWay, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH = max(dot(normal, halfWay), 0.0);
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
    float alphaValue = 1.0f;
    float depthValue = texture(depthTex, texCoordinates).r;
    vec4 albedoAndEmissive = texture(albedoAndEmissiveTex, texCoordinates);
    vec4 normalAndAmbient = texture(normalAndAmbientTex, texCoordinates);

    vec4 worldPosition = fetchWorldPosition(texCoordinates, depthValue);
    vec3 albedo = albedoAndEmissive.rgb;
    float emissiveFactor = albedoAndEmissive.a * MAX_EMISSIVE_FACTOR; //unpack emissive factor
    float modelAmbientFactor = normalAndAmbient.a;

    if (modelAmbientFactor < 0.9999) { //apply lighting
        vec2 sceneSize = textureSize(depthTex, 0);
        vec3 vertexToCameraPos = normalize(positioningData.viewPosition - vec3(worldPosition));
        vec3 normal = normalize(vec3(normalAndAmbient) * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
        vec3 modelAmbient = albedo * modelAmbientFactor;

        fragColor = vec4(lightsData.globalAmbient, alphaValue); //start with global ambient

        if (sceneInfo.hasAmbientOcclusion) {
            float ambientOcclusionFactor = texture(ambientOcclusionTex, texCoordinates).r * AO_STRENGTH;
            fragColor.rgb -= vec3(ambientOcclusionFactor, ambientOcclusionFactor, ambientOcclusionFactor); //subtract ambient occlusion
        }

        const vec3 dielectricSurfacesBaseReflectivity = vec3(0.04); //value is a mean of all no-metallic surfaces (plastic, water, ruby, diamond, glass...)
        uvec2 materialValues = texelFetch(materialAndMaskTex, ivec2(texCoordinates * sceneSize), 0).rg;
        float roughness = float(materialValues.r) / 255.0;
        float metallic = float(materialValues.g) / 255.0;
        vec3 baseReflectivity = mix(dielectricSurfacesBaseReflectivity, albedo, metallic);

        fragColor.rgb += albedo * emissiveFactor; //add emissive lighting

        for (int lightIndex = 0, shadowLightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
            if (!lightsData.lightsInfo[lightIndex].isExist) {
                break;//no more light
            }

            LightValues lightValues = computeLightValues(lightsData.lightsInfo[lightIndex], normal, vec3(worldPosition));
            float lightAttenuation = reduceColorBanding(lightValues.lightAttenuation, 0.008);

            vec3 lightRadiance = lightsData.lightsInfo[lightIndex].lightColor * lightAttenuation;
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
            if (sceneInfo.hasShadow && (lightsData.lightsInfo[lightIndex].lightFlags & LIGHT_FLAG_PRODUCE_SHADOW) != 0) {
                float shadowQuantity = 0.0f;
                if (lightsData.lightsInfo[lightIndex].lightType == 0) { //sun
                    shadowQuantity = computeSunShadowQuantity(shadowLightIndex, worldPosition, lightValues.NdotL);
                } else if (lightsData.lightsInfo[lightIndex].lightType == 1) { //omnidirectional
                    vec3 lightPosition = lightsData.lightsInfo[lightIndex].position;
                    shadowQuantity = computeOmnidirectionalShadowQuantity(shadowLightIndex, worldPosition, lightValues.NdotL, lightPosition);
                } else if (lightsData.lightsInfo[lightIndex].lightType == 2) { //spot
                    shadowQuantity = computeSpotShadowQuantity(shadowLightIndex, worldPosition, lightValues.NdotL);
                }

                shadowAttenuation = 1.0 - (shadowQuantity * lightsData.lightsInfo[lightIndex].shadowStrength);

                shadowLightIndex++;
            }

            //add ambient
            fragColor.rgb += modelAmbient * lightAttenuation;

            //update with PBR formula
            vec3 pbrFragColor = bidirectionalReflectanceDist * lightRadiance * lightValues.NdotL;
            fragColor.rgb += shadowAttenuation * pbrFragColor;
        }
    } else { //do not apply lighting (e.g. skybox, geometry models...)
        fragColor.rgba = vec4(albedo * (1.0 + emissiveFactor), alphaValue); //albedo + add emissive lighting
    }

    fragColor.rgb = addFog(fragColor.rgb, worldPosition);

    //DEBUG: add color to shadow map splits
    /* const float colorValue = 0.25;
    vec4 splitColors[5] = vec4[](
        vec4(colorValue, 0.0, 0.0, 1.0), vec4(0.0, colorValue, 0.0, 1.0), vec4(0.0, 0.0, colorValue, 1.0),
        vec4(colorValue, 0.0, colorValue, 1.0), vec4(colorValue, colorValue, 0.0, 1.0));
    for (int i = 0; i < MAX_SPLIT_SHADOW_MAPS; ++i) {
        float frustumCenterDist = distance(vec3(worldPosition), shadowMapData.splitData[i].xyz);
        float frustumRadius = shadowMapData.splitData[i].w;
        if (frustumCenterDist < frustumRadius) {
            fragColor += splitColors[i % 5];
            break;
        }
    }*/
}
