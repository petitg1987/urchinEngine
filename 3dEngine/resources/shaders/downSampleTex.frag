#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define OUTPUT_TYPE vec3
#define SOURCE_TEX_COMPONENTS rgb

uniform sampler2D tex;

in vec2 vertexTextCoordinates;

layout (location = 0) out OUTPUT_TYPE fragColor;

void main() {
    fragColor = texture2D(tex, vertexTextCoordinates.st).SOURCE_TEX_COMPONENTS;
}