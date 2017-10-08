#version 330

in vec3 normal;

uniform float ambient;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	fragColor = vec4(1.0, 1.0, 1.0, 0.0);

	//material
	vec3 texNormal = (normal + 1.0) / 2.0;
	fragNormalAndAmbient = vec4(texNormal, ambient);
}
