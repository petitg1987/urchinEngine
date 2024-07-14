#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D inputTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 color = texture(inputTex, texCoordinates).rgb;
    fragColor = vec4(color, 1.0);
}
