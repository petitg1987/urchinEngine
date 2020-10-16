#version 450
#extension GL_EXT_gpu_shader4 : enable

//values are replaced at compilation time:
#define OUTPUT_TYPE vec3
#define SOURCE_TEX_COMPONENTS rgb

uniform sampler2DArray tex;

in int gl_Layer;
in vec2 vertexTextCoordinates;

layout (location = 0) out OUTPUT_TYPE fragColor;

void main() {
    fragColor = texture2DArray(tex, vec3(vertexTextCoordinates.st, gl_Layer)).SOURCE_TEX_COMPONENTS;
}