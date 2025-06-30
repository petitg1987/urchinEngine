#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D depthTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    fragColor = vec2(0.0, 0.0);
}
