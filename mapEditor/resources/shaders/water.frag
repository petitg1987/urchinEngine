#version 330

in vec2 textCoordinates;

uniform sampler2D normalTex;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = vec4(1.0, 0.0, 0.0, 0.5);

	//normal and ambient factor
	vec4 normal = texture2D(normalTex, textCoordinates);
	fragNormalAndAmbient = vec4(normal.xyz, 0.5);
}
