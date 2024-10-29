#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform ColorParams {
    float gammaFactor;
} colorParams;
layout(binding = 1) uniform sampler2D sceneTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 color = texture(sceneTex, texCoordinates).xyz;
    fragColor = vec4(color * colorParams.gammaFactor, 1.0);
}
