#version 330

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = vec4(1.0, 0.0, 0.0, 0.5);

	//material
	fragNormalAndAmbient.a = 1.0; //no lighting
}
