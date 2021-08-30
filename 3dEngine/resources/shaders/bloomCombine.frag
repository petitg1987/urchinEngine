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

vec3 toneMapping(vec3 hdrColor) { //TODO check
    float exposure = 0.7;
    vec3 color = hdrColor * exposure;

    //no tone mapping
    //return clamp(hdrColor, 0.0, 1.0);

    //ACES tone mapping
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((color * ( a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main() {
    vec3 bloomValue;
    if (QUALITY_TEXTURE_FETCH) {
        bloomValue = upSample9Fetch(lastBloomStepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    } else {
        bloomValue = upSample4Fetch(lastBloomStepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    }
    vec3 hdrColor = texture(inputHdrTexture, texCoordinates).rgb;

    vec3 ldrColor = toneMapping(hdrColor + bloomValue);
    fragColor = vec4(ldrColor, 1.0);
}
