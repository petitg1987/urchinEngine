#version 330

in vec2 textCoordinates;

uniform sampler2D colorTex;
uniform float colorFactor;

out vec4 fragColor;

void main()
{
	vec4 diffuse = texture2D(colorTex, textCoordinates);
	
	vec4 finalDiffuse;
	finalDiffuse.x = pow(diffuse.x, colorFactor);
	finalDiffuse.y = pow(diffuse.y, colorFactor);
	finalDiffuse.z = pow(diffuse.z, colorFactor);
	finalDiffuse.w = diffuse.w;
	
	fragColor = finalDiffuse;
}