#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_samplingFunctions.frag"

layout(constant_id = 0) const bool QUALITY_TEXTURE_FETCH = true;
layout(constant_id = 1) const float SAMPLE_SCALE = 1.0;

layout(std140, set = 0, binding = 0) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 1) uniform sampler2D lastBloomStepTexture;
layout(binding = 2) uniform sampler2D inputHdrTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 bloomValue;
    if (QUALITY_TEXTURE_FETCH) {
        bloomValue = upSample9Fetch(lastBloomStepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    } else {
        bloomValue = upSample4Fetch(lastBloomStepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    }
    vec3 hdrValue = texture(inputHdrTexture, texCoordinates).rgb;

    fragColor = vec4(hdrValue + bloomValue, 1.0);
}
