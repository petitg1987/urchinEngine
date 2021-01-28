#version 450
#extension GL_ARB_separate_shader_objects : enable

in vec2 textCoordinates;

uniform sampler2D diffuseTexture;

layout (location = 0) out vec4 fragColor;

void main() {
    fragColor = texture2D(diffuseTexture, textCoordinates);
}
