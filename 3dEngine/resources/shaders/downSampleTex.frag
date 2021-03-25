#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define OUTPUT_TYPE vec3
#define SOURCE_TEX_COMPONENTS rgb

layout(binding = 20) uniform sampler2D tex;

layout(location = 0) in vec2 vertexTexCoordinates;

layout(location = 0) out OUTPUT_TYPE fragColor;

void main() {
    fragColor = texture2D(tex, vertexTexCoordinates.st).SOURCE_TEX_COMPONENTS;
}