#version 450

in vec2 textCoordinates;

uniform sampler2D diffuseTexture;

layout (location = 0) out vec4 fragColor;

void main(){
    fragColor = texture2D(diffuseTexture, textCoordinates);
}
