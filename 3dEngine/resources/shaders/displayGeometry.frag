#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 1) uniform Visual {
    vec4 color;
} visual;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    fragColor = visual.color;

    //material
    fragNormalAndAmbient.a = 1.0; //no lighting
}
