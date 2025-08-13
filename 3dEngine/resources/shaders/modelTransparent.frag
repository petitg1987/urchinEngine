#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const uint MAX_LIGHTS = 1000; //must be equals to LightManager::MAX_LIGHTS
layout(constant_id = 1) const float MAX_EMISSIVE_FACTOR = 0.0;

//global
layout(std140, set = 0, binding = 1) uniform MeshData {
    uint lightMask;
    float encodedEmissiveFactor; //encoded between 0.0 (no emissive) and 1.0 (max emissive)
    float ambientFactor;
} meshData;
layout(std140, set = 0, binding = 2) uniform CameraPlanes {
    float nearPlane;
    float farPlane;
} cameraPlanes;

//lighting
layout(std430, set = 0, binding = 4) readonly buffer LightsData {
    vec3 globalAmbient;
    uint lightsCount;
    LightInfo lightsInfo[MAX_LIGHTS];
} lightsData;

//texture
layout(binding = 5) uniform sampler2D albedoTex;
layout(binding = 6) uniform sampler2D normalTex;
layout(binding = 7) uniform sampler2D roughnessTex; //usage of this texture not implemented
layout(binding = 8) uniform sampler2D metalnessTex; //usage of this texture not implemented

layout(location = 0) in vec3 t;
layout(location = 1) in vec3 b;
layout(location = 2) in vec3 n;
layout(location = 3) in vec2 texCoordinates;
layout(location = 4) in vec4 worldPosition;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 albedo = texture(albedoTex, texCoordinates);
    if (albedo.a < 0.01) {
        discard;
    }

    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(vec3(texture(normalTex, texCoordinates)) * 2.0 - 1.0);
    vec3 normal = tbnMatrix * texNormal;
    float emissiveFactor = meshData.encodedEmissiveFactor * MAX_EMISSIVE_FACTOR;

    if (meshData.lightMask != 0) { //apply lighting
        vec3 modelAmbient = albedo.rgb * meshData.ambientFactor;
        fragColor = vec4(lightsData.globalAmbient, albedo.a);

        for (int lightIndex = 0; lightIndex < lightsData.lightsCount; ++lightIndex) {
            LightInfo lightInfo = lightsData.lightsInfo[lightIndex];

            if ((lightInfo.lightMask & meshData.lightMask) == 0) {
                continue; //no lighting on this mesh
            }

            LightValues lightValues = computeLightValues(lightInfo, normal, vec3(worldPosition));
            float lightAttenuation = reduceColorBanding(lightValues.lightAttenuation, 0.007);

            vec3 ambient = lightInfo.lightColor * modelAmbient;

            fragColor.rgb += lightAttenuation * ((albedo.rgb * lightValues.NdotL) + ambient);
        }
        fragColor.rgb += albedo.rgb * emissiveFactor;
    } else { //do not apply lighting
        fragColor = vec4(albedo.rgb * (1.0 + emissiveFactor), albedo.a); //albedo + add emissive lighting
    }
}
