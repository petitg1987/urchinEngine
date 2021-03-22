#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 20) uniform sampler2D diffuseTexture;

layout(location = 0) in vec2 textCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = texture(diffuseTexture, textCoordinates);
}
