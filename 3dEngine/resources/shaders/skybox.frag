#version 330

in vec3 textCoordinates;

uniform samplerCube diffuseTexture;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = texture(diffuseTexture, textCoordinates);

	//material
	fragNormalAndAmbient.a = 1.0; //no lighting
}
