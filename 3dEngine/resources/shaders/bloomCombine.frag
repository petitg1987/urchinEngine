#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D lightingPassTexture;
layout(binding = 1) uniform sampler2D intermediateFullResTexture; //TODO rename !

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 lightingPassValue = texture(lightingPassTexture, texCoordinates).rgb;
    vec3 intermediateFullResValue = texture(intermediateFullResTexture, texCoordinates).rgb;

    fragColor = vec4(lightingPassValue + intermediateFullResValue, 1.0);
}
