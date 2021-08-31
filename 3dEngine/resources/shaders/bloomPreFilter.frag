#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_samplingFunctions.frag"

layout(constant_id = 0) const bool QUALITY_TEXTURE_FETCH = true;

layout(std140, set = 0, binding = 0) uniform PreFilterTweak {
    float threshold;
    vec3 softCurveParams;
} preFilterTweak;
layout(std140, set = 0, binding = 1) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 2) uniform sampler2D inputHdrTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

float computeSoftContribution(float brightness) {
    //See /scene/renderer3d/postprocess/bloom/_doc/bloomPrefilterFormula.ods for formula details
    float underThresholdContribution = preFilterTweak.softCurveParams.z * pow(clamp(brightness - preFilterTweak.softCurveParams.x, 0.0, preFilterTweak.softCurveParams.y), 2.0);
    return max(underThresholdContribution, brightness - preFilterTweak.threshold) / max(brightness, 0.0001);
}

void main() {
    vec3 hdrColor;
    if (QUALITY_TEXTURE_FETCH) {
        hdrColor = downSampleBlur13Fetch(inputHdrTexture, texCoordinates, tex.texelSize);
    } else {
        hdrColor = downSampleBlur4Fetch(inputHdrTexture, texCoordinates, tex.texelSize);
    }

    float brightness = max(max(hdrColor.x, hdrColor.y), hdrColor.z);
    float contribution = computeSoftContribution(brightness);

    fragColor = vec4(hdrColor * contribution, 1.0);
}
