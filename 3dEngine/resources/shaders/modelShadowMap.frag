#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 fragColor;

void main() {
    float depth = gl_FragCoord.z; //from 0.0 (near plane) to 1.0 (far plane)
    fragColor.x = depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);
    fragColor.y = (depth * depth) + 0.25 * (dx * dx + dy * dy);
}
