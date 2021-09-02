#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_samplingFunctions.frag"

layout(constant_id = 0) const uint QUALITY_TEXTURE_FETCH = 0;
layout(constant_id = 1) const float SAMPLE_SCALE = 1.0;

layout(std140, set = 0, binding = 0) uniform Tex {
    vec2 texelSize;
} tex;
layout(std140, set = 0, binding = 1) uniform Exposure {
    float factor;
} exposure;
layout(binding = 2) uniform sampler2D lastBloomStepTexture;
layout(binding = 3) uniform sampler2D inputHdrTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec3 toneMapping(vec3 hdrColor) {
    vec3 color = hdrColor * exposure.factor;

    //null tone mapping
    return clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}

void main() {
    vec3 bloomValue;
    if (QUALITY_TEXTURE_FETCH == 0) {
        bloomValue = upSample9Fetch(lastBloomStepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    } else if (QUALITY_TEXTURE_FETCH == 1) {
        bloomValue = upSample4Fetch(lastBloomStepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    } else if (QUALITY_TEXTURE_FETCH == 2) {
        bloomValue = texture(lastBloomStepTexture, texCoordinates).rgb;
    }
    vec3 hdrColor = texture(inputHdrTexture, texCoordinates).rgb;

    vec3 ldrColor = toneMapping(hdrColor + bloomValue);
    fragColor = vec4(ldrColor, 1.0);
}
