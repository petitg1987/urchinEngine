#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float AMBIENT = 0.4;

layout(std140, set = 0, binding = 2) uniform ColorParams {
    float alphaFactor;
    float gammaFactor;
} colorParams;
layout(std140, set = 0, binding = 3) uniform CameraInfo {
    vec2 jitterInPixel;
} cameraInfo;
layout(binding = 4) uniform sampler2D albedoTex;

layout(location = 0) in vec2 texCoordinates;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out uvec4 fragPbrAndMask;

vec2 unjitterTextureUv(vec2 uv) {
    //Tips to debug the following code: increase the camera jittering of 50.0f and check that textures don't jitter despite the camera jittering
    return uv - (dFdxFine(uv) * cameraInfo.jitterInPixel.x) - (dFdyFine(uv) * cameraInfo.jitterInPixel.y);
}

void main() {
    //albedo and emissive
    vec2 unjitterUv = unjitterTextureUv(texCoordinates);
    vec4 color = texture(albedoTex, unjitterUv);
    if (color.a < 0.05) {
        discard;
    }
    color.a *= colorParams.alphaFactor;
    color.rgb *= colorParams.gammaFactor;
    fragAlbedoAndEmissive = color; //Write color with alpha to compute the blend on the RGB channels. The alpha channel (=emissive) will stay unchanged thanks to the configured alpha blend functions.

    //ambient factor
    vec3 encodedNormal = (normalize(normal) + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(encodedNormal, AMBIENT);

    //pbr & mask
    fragPbrAndMask.r = 255; //roughness
    fragPbrAndMask.g = 0; //metalness
    fragPbrAndMask.b = 255; //light mask
}
