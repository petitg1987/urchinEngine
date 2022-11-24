#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"

layout(std140, set = 0, binding = 2) uniform ColorParams {
    float alphaFactor;
    float gammaFactor;
} colorParams;
layout(binding = 3) uniform sampler2D albedoTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = texture(albedoTex, texCoordinates);
    fragColor.a *= colorParams.alphaFactor;
    fragColor = applyGammaCorrection(fragColor, colorParams.gammaFactor);
}
