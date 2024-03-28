#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out float fragColor;

void main() {
    fragColor = gl_FragCoord.z; //from 0.0 (near plane) to 1.0 (far plane)
}
