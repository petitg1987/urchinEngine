#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_samplingFunctions.frag"

layout(constant_id = 0) const bool QUALITY_TEXTURE_FETCH = true;

layout(std140, set = 0, binding = 0) uniform PreFilterTweak {
    float threshold;
} preFilterTweak;
layout(std140, set = 0, binding = 1) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 2) uniform sampler2D inputHdrTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 hdrColor;
    if (QUALITY_TEXTURE_FETCH) {
        hdrColor = downSampleBlur13Fetch(inputHdrTexture, texCoordinates, tex.texelSize);
    } else {
        hdrColor = downSampleBlur4Fetch(inputHdrTexture, texCoordinates, tex.texelSize);
    }

    //See following link for the formula: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/
    //Note that more advanced formula to soft edges is also explained under "Soft Threshold" section.
    float brightness = max(max(hdrColor.x, hdrColor.y), hdrColor.z);
    float contribution = max(0.0, brightness - preFilterTweak.threshold) / max(brightness, 0.00001);
    fragColor = vec4(hdrColor * contribution, 1.0);
}
