#version 330

in vec3 t, b, n;
in vec2 textCoordinates;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform float ambientFactor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main()
{
	//diffuse
	fragColor = texture2D(diffuseTex, textCoordinates);
	
	//normal and ambient factor
	mat3 mInverseTBN = mat3(normalize(t), normalize(b),	normalize(n));
	vec3 normal = normalize(vec3(texture2D(normalTex, textCoordinates)) * 2.0 - 1.0);
	normal = ((mInverseTBN * normal) + 1.0) / 2.0;
	fragNormalAndAmbient = vec4(normal, ambientFactor);
}
