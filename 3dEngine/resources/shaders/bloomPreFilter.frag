#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D lightingPassTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 lightTexValue = texture(lightingPassTexture, texCoordinates).rgb;

    float threshold = 1.0;

    float brightness = max(max(lightTexValue.x, lightTexValue.y), lightTexValue.z);
    float contribution = max(0.0, brightness - threshold);
    contribution /= max(brightness, 0.00001);

    fragColor = vec4(lightTexValue * contribution, 1.0);
}
