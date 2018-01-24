#version 330

in vec2 textCoordinates;

uniform sampler2D normalTex;
uniform float sumTimeStep;
uniform vec3 waterColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = vec4(waterColor, 0.5);

	//normal and ambient factor
	float shiftMove = sin(sumTimeStep) * 0.02f;
	vec4 normal = texture2D(normalTex, textCoordinates + vec2(shiftMove, shiftMove));
	fragNormalAndAmbient = vec4(normal.xyz, 0.5);
}
