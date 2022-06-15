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
layout(binding = 6) uniform sampler2D roughnessTex; //TODO use it ?
layout(binding = 7) uniform sampler2D metalnessTex;

layout(location = 0) in vec3 t;
layout(location = 1) in vec3 b;
layout(location = 2) in vec3 n;
layout(location = 3) in vec2 texCoordinates;
layout(location = 4) in vec4 worldPosition;

layout(location = 0) out vec4 accumulationTexture;
layout(location = 1) out float revealTexture;

void fillTransparentTextures(vec4 fragColor) {
    float depth = gl_FragCoord.z; //from 0.0 (near plane) to 1.0 (far plane)
    float linearizedDepth = (cameraPlanes.nearPlane * cameraPlanes.farPlane) /
            (depth * (cameraPlanes.nearPlane - cameraPlanes.farPlane) + cameraPlanes.farPlane); //linearized depth from near plane value to far plane value

    //Weight formulas from paper: http://jcgt.org/published/0002/02/09/
    //Info: formula (10) not work so well when the camera is close to the transparent models {weight = max(0.01, 3000.0 * pow(1.0 - depth, 3.0))}
    float weight = max(0.01, min(3000.0, 10.0 / (0.00001 + pow(abs(linearizedDepth) / 5.0, 2.0) + pow(abs(linearizedDepth) / 200.0, 6.0)))); //formula (7)

    accumulationTexture = vec4(fragColor.rgb * fragColor.a, fragColor.a) * weight;
    revealTexture = fragColor.a;
}

void main() {
    vec4 albedo = texture(albedoTex, texCoordinates);
    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(vec3(texture(normalTex, texCoordinates)) * 2.0 - 1.0);
    vec3 normal = tbnMatrix * texNormal;

    vec4 fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (materialData.ambientFactor < 0.9999) { //apply lighting
        float emissiveFactor = materialData.encodedEmissiveFactor * MAX_EMISSIVE_FACTOR;
        vec3 modelAmbient = vec3(albedo) * materialData.ambientFactor;
        fragColor = vec4(lightsData.globalAmbient, albedo.a);

        float emissiveAttenuation = max(0.0, 1.0 - emissiveFactor);//disable lighting on highly emissive objects (give better results)
        for (int lightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
            if (lightsData.lightsInfo[lightIndex].isExist) {
                LightValues lightValues = computeLightValues(lightsData.lightsInfo[lightIndex], normal, vec3(worldPosition));
                vec3 ambient = lightsData.lightsInfo[lightIndex].lightColor * modelAmbient;

                fragColor.rgb += emissiveAttenuation * lightValues.lightAttenuation * ((albedo.rgb * lightValues.NdotL) + ambient);
            } else {
                break;//no more light
            }
        }
        fragColor.rgb += albedo.rgb * emissiveFactor;
    } else { //do not apply lighting
        fragColor = albedo;
    }

    fillTransparentTextures(fragColor);
}
