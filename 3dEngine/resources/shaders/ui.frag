#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D diffuseTexture; //binding 20

in vec2 textCoordinates;

layout (location = 0) out vec4 fragColor;

void main() {
    fragColor = texture2D(diffuseTexture, textCoordinates);
}
