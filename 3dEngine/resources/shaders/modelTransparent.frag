#version 450
#extension GL_ARB_separate_shader_objects : enable

const uint MAX_LIGHTS = 15; //must be equals to LightManager::LIGHTS_SHADER_SHADER_LIMIT

//global
layout(std140, set = 0, binding = 2) uniform MeshData {
    mat4 mNormal;
    float ambientFactor;
} meshData;
layout(std140, set = 0, binding = 3) uniform CameraPlanes {
    float nearPlane;
    float farPlane;
} cameraPlanes;

//lighting
struct LightInfo {
    bool isExist;
    bool produceShadow;
    bool hasParallelBeams;
    vec3 positionOrDirection;
    float exponentialAttenuation;
    vec3 lightAmbient;
};
layout(std140, set = 0, binding = 4) uniform LightsData {
    LightInfo lightsInfo[MAX_LIGHTS];
    vec3 globalAmbient;
} lightsData;

//texture
layout(binding = 5) uniform sampler2D diffuseTex;
layout(binding = 6) uniform sampler2D normalTex;

layout(location = 0) in vec3 t;
layout(location = 1) in vec3 b;
layout(location = 2) in vec3 n;
layout(location = 3) in vec2 texCoordinates;
layout(location = 4) in vec4 position; //TODO check it is correct. Is position vary ?

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
    vec4 diffuse = texture(diffuseTex, texCoordinates);

    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(vec3(texture(normalTex, texCoordinates)) * 2.0 - 1.0);
    vec3 normal = tbnMatrix * texNormal;

    //lighting
    vec3 modelAmbient = vec3(diffuse) * meshData.ambientFactor;
    vec4 fragColor = vec4(lightsData.globalAmbient, 1.0f);

    for (int lightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
        if (lightsData.lightsInfo[lightIndex].isExist) {
            vec3 vertexToLightNormalized;
            float lightAttenuation;

            if (lightsData.lightsInfo[lightIndex].hasParallelBeams) { //sun light
                vec3 vertexToLight = -lightsData.lightsInfo[lightIndex].positionOrDirection;
                vertexToLightNormalized = normalize(vertexToLight);
                lightAttenuation = 1.0f;
            } else { //omnidirectional light
                vec3 vertexToLight = lightsData.lightsInfo[lightIndex].positionOrDirection - vec3(position);
                float dist = length(vertexToLight);
                vertexToLightNormalized = normalize(vertexToLight);
                lightAttenuation = exp(-dist * lightsData.lightsInfo[lightIndex].exponentialAttenuation);
            }

            float NdotL = max(dot(normal, vertexToLightNormalized), 0.0f);
            vec4 ambient = vec4(lightsData.lightsInfo[lightIndex].lightAmbient * modelAmbient, 1.0);

            fragColor += lightAttenuation * ((diffuse * NdotL) + ambient);
        } else {
            break; //no more light
        }
    }

    fillTransparentTextures(fragColor);
}
