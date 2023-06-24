#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const uint MAX_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_LIMIT
layout(constant_id = 1) const float MAX_EMISSIVE_FACTOR = 0.0;

//global
layout(std140, set = 0, binding = 1) uniform MaterialData {
    float encodedEmissiveFactor; //encoded between 0.0 (no emissive) and 1.0 (max emissive)
    float ambientFactor;
} materialData;
layout(std140, set = 0, binding = 2) uniform CameraPlanes {
    float nearPlane;
    float farPlane;
} cameraPlanes;

//lighting
layout(std140, set = 0, binding = 3) uniform LightsData {
    LightInfo lightsInfo[MAX_LIGHTS];
    vec3 globalAmbient;
} lightsData;

//texture
layout(binding = 4) uniform sampler2D albedoTex;
layout(binding = 5) uniform sampler2D normalTex;
layout(binding = 6) uniform sampler2D roughnessTex; //usage of this texture not implemented
layout(binding = 7) uniform sampler2D metalnessTex; //usage of this texture not implemented

layout(location = 0) in vec3 t;
layout(location = 1) in vec3 b;
layout(location = 2) in vec3 n;
layout(location = 3) in vec2 texCoordinates;
layout(location = 4) in vec4 worldPosition;

layout(location = 0) out vec4 fragColor;

void main() { //TODO review
    vec4 albedo = texture(albedoTex, texCoordinates);
    if (albedo.a < 0.01) {
        discard;
    }

    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(vec3(texture(normalTex, texCoordinates)) * 2.0 - 1.0);
    vec3 normal = tbnMatrix * texNormal;
    float emissiveFactor = materialData.encodedEmissiveFactor * MAX_EMISSIVE_FACTOR;

    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (materialData.ambientFactor < 0.9999) { //apply lighting
        vec3 modelAmbient = albedo.rgb * materialData.ambientFactor;
        fragColor = vec4(lightsData.globalAmbient, albedo.a);

        for (int lightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
            if (lightsData.lightsInfo[lightIndex].isExist) {
                LightValues lightValues = computeLightValues(lightsData.lightsInfo[lightIndex], normal, vec3(worldPosition));
                vec3 ambient = lightsData.lightsInfo[lightIndex].lightColor * modelAmbient;

                fragColor.rgb += lightValues.lightAttenuation * ((albedo.rgb * lightValues.NdotL) + ambient);
            } else {
                break; //no more light
            }
        }
        fragColor.rgb += albedo.rgb * emissiveFactor;
    } else { //do not apply lighting
        fragColor = vec4(albedo.rgb * (1.0 + emissiveFactor), albedo.a); //albedo + add emissive lighting
    }
}
