#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 2) uniform Alpha {
    float factor;
} alpha;
layout(binding = 3) uniform sampler2D diffuseTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = texture(diffuseTexture, texCoordinates);
    fragColor.a *= alpha.factor;
}
