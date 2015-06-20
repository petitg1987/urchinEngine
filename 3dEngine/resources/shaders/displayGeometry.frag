#version 330

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

uniform vec3 color;

void main()
{
	//diffuse
	fragColor = vec4(color.r, color.g, color.b, 0.0);

	//material
	fragNormalAndAmbient.a = 1.0; //no lighting
}
