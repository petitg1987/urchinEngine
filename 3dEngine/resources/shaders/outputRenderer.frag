#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float GAMMA_FACTOR = 0.0;

layout(binding = 0) uniform sampler2D sceneTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 color = texture(sceneTex, texCoordinates).xyz;
    fragColor = vec4(color * GAMMA_FACTOR, 1.0);
}
