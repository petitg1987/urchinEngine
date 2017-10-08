#version 330

in vec3 normal;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = vec4(1.0, 1.0, 1.0, 0.0);

	//material
	fragNormalAndAmbient = vec4(normal, 0.5); //TODO hard coded value
}
