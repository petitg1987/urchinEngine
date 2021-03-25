#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform vec4 color; //binding 1

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    fragColor = vec4(color.r, color.g, color.b, color.a);

    //material
    fragNormalAndAmbient.a = 1.0; //no lighting
}
