#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 1) uniform MaterialData {
    float encodedEmissiveFactor; //encoded between 0.0 (no emissive) and 1.0 (max emissive)
    float ambientFactor;
} materialData;

layout(std140, set = 0, binding = 2) uniform CameraInfo {
    vec2 jitterInPixel;
} cameraInfo;

layout(binding = 4) uniform sampler2D albedoTex;
layout(binding = 5) uniform sampler2D normalTex;
layout(binding = 6) uniform sampler2D roughnessTex;
layout(binding = 7) uniform sampler2D metalnessTex;

layout(location = 0) in vec3 t;
layout(location = 1) in vec3 b;
layout(location = 2) in vec3 n;
layout(location = 3) in vec2 texCoordinates;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out vec2 fragPbr;

vec2 unjitterTextureUv(vec2 uv) {
    //Tips to debug the following code: increase the camera jittering of 50.0f and check that textures don't jitter despite the camera jittering
    return uv - (dFdxFine(uv) * cameraInfo.jitterInPixel.x) - (dFdyFine(uv) * cameraInfo.jitterInPixel.y);
}

void main() {
    vec2 unjitterUv = unjitterTextureUv(texCoordinates);

    //albedo and emissive
    fragAlbedoAndEmissive = vec4(texture(albedoTex, unjitterUv).rgb, materialData.encodedEmissiveFactor);

    //normal and ambient factor
    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(texture(normalTex, unjitterUv).rgb * 2.0 - 1.0);
    vec3 normal = ((tbnMatrix * texNormal) + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(normal, materialData.ambientFactor);

    //pbr
    fragPbr.r = texture(roughnessTex, unjitterUv).r;
    fragPbr.g = texture(metalnessTex, unjitterUv).r;
}
