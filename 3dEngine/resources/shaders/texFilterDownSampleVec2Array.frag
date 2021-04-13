#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 20) uniform sampler2DArray tex;

layout(location = 0) in vec2 vertexTexCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    fragColor = texture(tex, vec3(vertexTexCoordinates.st, gl_Layer)).rg;
}