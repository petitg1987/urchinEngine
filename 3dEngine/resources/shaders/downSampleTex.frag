#version 330

uniform sampler2D tex;

in vec2 vertexTextCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = texture2D(tex, vertexTextCoordinates.st).#SOURCE_TEX_COMPONENTS#;
}