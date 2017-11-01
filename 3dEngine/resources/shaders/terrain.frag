#version 330

in vec2 textCoordinates;
in vec3 normal;

uniform sampler2D diffuseTex;
uniform float ambient;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = texture2D(diffuseTex, textCoordinates);

	//material
	vec3 texNormal = (normal + 1.0) / 2.0;
	fragNormalAndAmbient = vec4(texNormal, ambient);
}
