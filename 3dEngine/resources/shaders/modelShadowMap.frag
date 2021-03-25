#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 fragColor;

void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    fragColor.x = depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);
    fragColor.y = (depth * depth) + 0.25*(dx*dx + dy*dy);
}