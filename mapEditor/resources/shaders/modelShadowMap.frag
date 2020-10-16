#version 450

layout (location = 0) out vec2 fragColor;

void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    fragColor.x = depth;

    float dx = dFdx(depth);
    float dy = dFdy(depth);
    fragColor.y = (depth * depth) + 0.25*(dx*dx + dy*dy);
}