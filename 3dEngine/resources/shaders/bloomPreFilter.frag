#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform BloomTweak {
    float threshold;
} bloomTweak;
layout(binding = 1) uniform sampler2D lightingPassTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 lightTexValue = texture(lightingPassTexture, texCoordinates).rgb;

    //See following link for the formula: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/
    //Note that more advanced formula to soft edges is also explained under "Soft Threshold" section.
    float brightness = max(max(lightTexValue.x, lightTexValue.y), lightTexValue.z);
    float contribution = max(0.0, brightness - bloomTweak.threshold) / max(brightness, 0.00001);
    fragColor = vec4(lightTexValue * contribution, 1.0);
}
