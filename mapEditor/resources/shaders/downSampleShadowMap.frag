#version 330
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray shadowMapTex;

in int gl_Layer;
in vec2 textCoordinates;

layout (location = 0) out vec2 fragColor;

void main(){
	fragColor = texture2DArray(shadowMapTex, vec3(textCoordinates.st, gl_Layer)).rg;
}