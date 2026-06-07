#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const uint MAX_LIGHTS = 1000; //must be equals to LightManager::MAX_LIGHTS
layout(constant_id = 1) const float AO_STRENGTH = 0.0;
layout(constant_id = 2) const uint MAX_LIGHTS_WITH_SHADOW = 10; //must be equals to ShadowManager::LIGHTS_WITH_SHADOW_SHADER_LIMIT
layout(constant_id = 3) const uint MAX_SPLIT_SHADOW_MAPS = 6; //must be equals to ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT
layout(constant_id = 4) const float SHADOW_MAP_NORMAL_BIAS_CONSTANT_FACTOR = 0.0; //normal offset bias expressed in shadow texels
layout(constant_id = 5) const float SHADOW_MAP_NORMAL_BIAS_SLOPE_FACTOR = 0.0; //extra normal offset bias (in texels) at grazing light angles
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
layout(std430, set = 0, binding = 2) readonly buffer LightsData {
    vec3 globalAmbient;
    uint lightsCount;
    LightInfo lightsInfo[MAX_LIGHTS];
} lightsData;

//shadow
layout(std140, set = 0, binding = 3) uniform ShadowMatrix {
    mat4 mLightProjectionView[MAX_LIGHTS_WITH_SHADOW * MAX_SPLIT_SHADOW_MAPS]; //use 1 dim. table because 2 dim. tables are bugged (only in RenderDoc ?)
} shadowMatrix;
layout(std140, set = 0, binding = 4) uniform ShadowInfo {
    vec4 splitData[MAX_SPLIT_SHADOW_MAPS];
    int offsetSampleCount;
} shadowInfo;

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
layout(binding = 9) uniform usampler2D materialAndMaskTex; //roughness (8 bits) + metalness (8 bits) + light mask (8 bits)
layout(binding = 10) uniform sampler2D ambientOcclusionTex; //ambient occlusion (8 or 16 bits)
layout(binding = 11) uniform sampler2DArray shadowMapTex[MAX_LIGHTS_WITH_SHADOW]; //shadow maps for each lights (32 bits * nbSplit * nbLight)
layout(binding = 12) uniform sampler2DArray shadowMapOffsetTex; //shadow maps offset (32 bits)

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

/*
 * Find out how big a single shadow map texel is in world units at a specific location
 */
float computeSmTexelInWorldSize(mat4 mLightProjectionView, vec4 worldPosition, int shadowMapWidth) {
    vec4 positionInClipSpace = mLightProjectionView * worldPosition;

    //Clip gradient: if I move 1 unit in world space, how much does positionInClipSpace change?
    vec3 clipXGradient = vec3(mLightProjectionView[0][0], mLightProjectionView[1][0], mLightProjectionView[2][0]);
    vec3 clipYGradient = vec3(mLightProjectionView[0][1], mLightProjectionView[1][1], mLightProjectionView[2][1]);
    vec3 clipWGradient = vec3(mLightProjectionView[0][3], mLightProjectionView[1][3], mLightProjectionView[2][3]);

    //Apply quotient rule
    float invClipW2 = 1.0 / (positionInClipSpace.w * positionInClipSpace.w);
    vec3 ndcXGradient = (clipXGradient * positionInClipSpace.w - positionInClipSpace.x * clipWGradient) * invClipW2;
    vec3 ndcYGradient = (clipYGradient * positionInClipSpace.w - positionInClipSpace.y * clipWGradient) * invClipW2;

    float worldSizePerNdcX = 1.0 / max(length(ndcXGradient), 1e-8);
    float worldSizePerNdcY = 1.0 / max(length(ndcYGradient), 1e-8);
    float ndcSizePerTexel = 2.0 / float(shadowMapWidth); //NDC ranges from -1.0 to 1.0
    return 0.5 * (worldSizePerNdcX + worldSizePerNdcY) * ndcSizePerTexel;
}

float computeShadowQuantity(int shadowLightIndex, int splitShadowMapIndex, vec4 worldPosition, vec3 normal, float NdotL) {
    mat4 mLightProjectionView = shadowMatrix.mLightProjectionView[shadowLightIndex * MAX_SPLIT_SHADOW_MAPS + splitShadowMapIndex];

    //TODO review !!!
    //Normal offset bias: move the sample along the surface normal by a fraction of the shadow texel world size. Because it is
    //applied in world space, it adapts to every projection (sun cascades, omnidirectional, spot) and to distance without per-light
    //tuning, and it scales with the light grazing angle (the main source of shadow acne) instead of biasing the depth comparison.
    int shadowMapWidth = textureSize(shadowMapTex[shadowLightIndex], 0).x;
    float smTexelInWorldSize = computeSmTexelInWorldSize(mLightProjectionView, worldPosition, shadowMapWidth);
    float grazingFactor = sqrt(clamp(1.0 - NdotL * NdotL, 0.0, 1.0)); //sin of the angle between the normal and the light
    float normalOffset = smTexelInWorldSize * (SHADOW_MAP_NORMAL_BIAS_CONSTANT_FACTOR + SHADOW_MAP_NORMAL_BIAS_SLOPE_FACTOR * grazingFactor);
    vec4 offsetWorldPosition = vec4(vec3(worldPosition) + normal * normalOffset, 1.0);

    vec4 shadowCoord = mLightProjectionView * offsetWorldPosition;
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.s = (shadowCoord.s / 2.0) + 0.5;
    shadowCoord.t = (shadowCoord.t / 2.0) + 0.5;

    const float SOFT_EDGE_LENGTH = 2.5f;
    float shadowMapInvSize = 1.0 / float(shadowMapWidth);
    vec2 sceneSize = textureSize(depthTex, 0);
    ivec2 offsetTexCoordinate = ivec2(texCoordinates * sceneSize) % ivec2(SHADOW_MAP_OFFSET_TEX_SIZE, SHADOW_MAP_OFFSET_TEX_SIZE); //TODO does offsetTexCoordinate can use smTexelInWorldSize ?

    //Phase 1: probe with reduced sample count to detect uniform regions
    int probeCount = min(5, shadowInfo.offsetSampleCount);
    int probeInShadow = 0;
    for (int i = 0; i < probeCount; ++i) {
        vec2 shadowMapOffset = texelFetch(shadowMapOffsetTex, ivec3(offsetTexCoordinate, i), 0).xy * SOFT_EDGE_LENGTH * shadowMapInvSize;
        float shadowDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st + shadowMapOffset, splitShadowMapIndex)).r;
        if (shadowCoord.z > shadowDepth) {
            probeInShadow++;
        }
    }

    //Early exit: fully lit or fully shadowed — no soft edge
    if (probeInShadow == 0) {
        return 0.0;
    } else if (probeInShadow == probeCount) {
        return 1.0;
    }

    //Phase 2: soft edge detected — run full sample set for accurate penumbra
    float shadowQuantity = float(probeInShadow);
    for (int i = probeCount; i < shadowInfo.offsetSampleCount; ++i) {
        vec2 shadowMapOffset = texelFetch(shadowMapOffsetTex, ivec3(offsetTexCoordinate, i), 0).xy * SOFT_EDGE_LENGTH * shadowMapInvSize;
        float shadowDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st + shadowMapOffset, splitShadowMapIndex)).r;
        if (shadowCoord.z > shadowDepth) {
            shadowQuantity += 1.0f;
        }
    }
    shadowQuantity = shadowQuantity / float(shadowInfo.offsetSampleCount);

    //DEBUG: fetch shadow map one time, no PCF filter
    /* shadowQuantity = 0.0f;
    float shadowDepth = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st, splitShadowMapIndex)).r;
    if (shadowCoord.z > shadowDepth) {
        shadowQuantity = 1.0f;
    } */

    return shadowQuantity;
}

float computeSunShadowQuantity(int shadowLightIndex, vec4 worldPosition, vec3 normal, float NdotL) {
    for (int splitShadowMapIndex = 0; splitShadowMapIndex < MAX_SPLIT_SHADOW_MAPS; ++splitShadowMapIndex) {
        float frustumCenterDist = distance(vec3(worldPosition), shadowInfo.splitData[splitShadowMapIndex].xyz);
        float frustumRadius = shadowInfo.splitData[splitShadowMapIndex].w;
        if (frustumCenterDist < frustumRadius) {
            return computeShadowQuantity(shadowLightIndex, splitShadowMapIndex, worldPosition, normal, NdotL);
        }
    }
    return 0.0;
}

float computeOmnidirectionalShadowQuantity(int shadowLightIndex, vec4 worldPosition, vec3 normal, float NdotL, vec3 lightPosition) {
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

    return computeShadowQuantity(shadowLightIndex, shadowMapIndex, worldPosition, normal, NdotL);
}

float computeSpotShadowQuantity(int shadowLightIndex, vec4 worldPosition, vec3 normal, float NdotL) {
    return computeShadowQuantity(shadowLightIndex, 0, worldPosition, normal, NdotL);
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
    vec2 sceneSize = textureSize(depthTex, 0);
    float depthValue = texture(depthTex, texCoordinates).r;
    vec4 albedoAndEmissive = texture(albedoAndEmissiveTex, texCoordinates);
    uvec3 materialAndMaskValues = texelFetch(materialAndMaskTex, ivec2(texCoordinates * sceneSize), 0).rgb;
    uint meshLightMask = materialAndMaskValues.b;

    vec4 worldPosition = fetchWorldPosition(texCoordinates, depthValue);
    vec3 albedo = albedoAndEmissive.rgb;
    float emissiveFactor = albedoAndEmissive.a * MAX_EMISSIVE_FACTOR; //unpack emissive factor

    if (meshLightMask != 0) { //apply lighting
        vec4 normalAndAmbient = texture(normalAndAmbientTex, texCoordinates);
        vec3 vertexToCameraPos = normalize(positioningData.viewPosition - vec3(worldPosition));
        vec3 normal = normalize(vec3(normalAndAmbient) * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
        vec3 modelAmbient = albedo * normalAndAmbient.a;

        //global ambient
        fragColor = vec4(lightsData.globalAmbient, 1.0);

        //indirect light (ambient or image based lighting & ambient occlusion)
        fragColor.rgb += modelAmbient;
        if (sceneInfo.hasAmbientOcclusion) {
            fragColor.rgb *= 1.0f - (texture(ambientOcclusionTex, texCoordinates).r * AO_STRENGTH);
        }

        //emissive light
        fragColor.rgb += albedo * emissiveFactor;

        const vec3 dielectricSurfacesBaseReflectivity = vec3(0.04); //value is a mean of all no-metallic surfaces (plastic, water, ruby, diamond, glass...)
        float roughness = float(materialAndMaskValues.r) / 255.0;
        float metallic = float(materialAndMaskValues.g) / 255.0;
        vec3 baseReflectivity = mix(dielectricSurfacesBaseReflectivity, albedo, metallic);

        for (int lightIndex = 0; lightIndex < lightsData.lightsCount; ++lightIndex) {
            LightInfo lightInfo = lightsData.lightsInfo[lightIndex];

            if ((lightInfo.lightMask & meshLightMask) == 0) {
                continue; //no lighting on this mesh
            }

            LightValues lightValues = computeLightValues(lightInfo, normal, vec3(worldPosition));
            float lightAttenuation = reduceColorBanding(lightValues.lightAttenuation, 0.02);

            vec3 lightRadiance = lightInfo.lightColor * lightAttenuation;
            vec3 bidirectionalReflectanceDist;
            if (lightInfo.isPbrEnabled) {
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
            if (sceneInfo.hasShadow && lightInfo.hasShadow) {
                float shadowQuantity = 0.0f;
                if (lightInfo.lightType == 0) { //sun
                    shadowQuantity = computeSunShadowQuantity(lightInfo.shadowLightIndex, worldPosition, normal, lightValues.NdotL);
                } else if (lightInfo.lightType == 1) { //omnidirectional
                    vec3 lightPosition = lightInfo.position;
                    shadowQuantity = computeOmnidirectionalShadowQuantity(lightInfo.shadowLightIndex, worldPosition, normal, lightValues.NdotL, lightPosition);
                } else if (lightInfo.lightType == 2) { //spot
                    shadowQuantity = computeSpotShadowQuantity(lightInfo.shadowLightIndex, worldPosition, normal, lightValues.NdotL);
                }
                shadowAttenuation = 1.0 - (shadowQuantity * lightInfo.shadowStrength);
            }

            //emissive correction (emissive materials generally don't reflect well the direct light)
            float lightingFactor = smoothstep(0.0, 1.0, 1.0 - emissiveFactor);

            //update with PBR formula
            vec3 pbrFragColor = bidirectionalReflectanceDist * lightRadiance * lightValues.NdotL;
            fragColor.rgb += shadowAttenuation * pbrFragColor * lightingFactor;
        }
    } else { //do not apply lighting (e.g. skybox, geometry models...)
        fragColor.rgba = vec4(albedo * (1.0 + emissiveFactor), 1.0); //albedo + add emissive lighting
    }

    fragColor.rgb = addFog(fragColor.rgb, worldPosition);

    //DEBUG: add color to shadow map splits
    /* const float colorValue = 0.25;
    vec4 splitColors[5] = vec4[](
        vec4(colorValue, 0.0, 0.0, 1.0), vec4(0.0, colorValue, 0.0, 1.0), vec4(0.0, 0.0, colorValue, 1.0),
        vec4(colorValue, 0.0, colorValue, 1.0), vec4(colorValue, colorValue, 0.0, 1.0));
    for (int i = 0; i < MAX_SPLIT_SHADOW_MAPS; ++i) {
        float frustumCenterDist = distance(vec3(worldPosition), shadowInfo.splitData[i].xyz);
        float frustumRadius = shadowInfo.splitData[i].w;
        if (frustumCenterDist < frustumRadius) {
            fragColor += splitColors[i % 5];
            break;
        }
    }*/
}
