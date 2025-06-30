#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D sceneTex;
layout(binding = 1) uniform sampler2D velocityTex;
layout(binding = 2) uniform sampler2D historyTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 currentColor = texture(sceneTex, texCoordinates).xyz;
    vec3 historyColor = texture(historyTex, texCoordinates).xyz;
    vec3 color = mix(currentColor, historyColor, 0.9);
    fragColor = vec4(color, 1.0);
}
