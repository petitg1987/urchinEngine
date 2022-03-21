#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2DArray tex;

layout(location = 0) in vec3 vertexTexCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    fragColor = texture(tex, vertexTexCoordinates).rg;
}