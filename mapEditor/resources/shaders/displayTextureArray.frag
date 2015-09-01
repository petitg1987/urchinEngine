#version 330
#extension GL_EXT_gpu_shader4 : enable

in vec2 textCoordinates;

uniform sampler2DArray colorTex;
uniform float colorFactor;
uniform int layer;

out vec4 fragColor;

void main(){
	vec4 diffuse = texture2DArray(colorTex, vec3(textCoordinates, layer));
	
	vec4 finalDiffuse;
	finalDiffuse.x = pow(diffuse.x, colorFactor);
	finalDiffuse.y = pow(diffuse.y, colorFactor);
	finalDiffuse.z = pow(diffuse.z, colorFactor);
	finalDiffuse.w = diffuse.w;
	
	fragColor = finalDiffuse;
}