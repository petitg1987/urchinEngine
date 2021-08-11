#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D diffuseTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 accumulationTexture;
layout(location = 1) out float revealTexture;

void main() {
    vec4 color = texture(diffuseTex, texCoordinates);

    float alpha = color.a;
    float weight = 1.0f; //TODO add weight formula

    accumulationTexture = vec4(color.rgb * alpha, alpha) * weight;
    revealTexture = alpha;
}
