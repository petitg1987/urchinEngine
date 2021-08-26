#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D inputHdrTexture;
layout(binding = 1) uniform sampler2D lastBloomStepTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 hdrValue = texture(inputHdrTexture, texCoordinates).rgb;
    vec3 bloomValue = texture(lastBloomStepTexture, texCoordinates).rgb;

    fragColor = vec4(hdrValue + bloomValue, 1.0);
}
