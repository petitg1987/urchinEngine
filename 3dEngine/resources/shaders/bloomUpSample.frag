#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D stepTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 texValue = texture(stepTexture, texCoordinates).rgb;

    fragColor = vec4(texValue, 1.0);
}
