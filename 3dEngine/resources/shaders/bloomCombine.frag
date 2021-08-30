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

vec3 toneMapping(vec3 hdrColor) {
    float exposure = 1.0; //0.7;
    vec3 color = hdrColor * exposure;


    //no tone mapping
    return clamp(hdrColor, 0.0, 1.0);

    //ACES tone mapping
//    float a = 2.51f;
//    float b = 0.03f;
//    float c = 2.43f;
//    float d = 0.59f;
//    float e = 0.14f;
//    return clamp((color * ( a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);

    //Uncharted 2
//    float gamma = 2.0;
//    float exposure = 1.5;
//    float A = 0.15;
//    float B = 0.50;
//    float C = 0.10;
//    float D = 0.20;
//    float E = 0.02;
//    float F = 0.30;
//    float W = 11.2;
//    vec3 color = hdrColor;
//    color *= exposure;
//    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
//    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
//    color /= white;
//    color = pow(color, vec3(1. / gamma));
//    return color;

    //simple reinhard
//    float gamma = 2.0;
//    float exposure = 1.0;
//    vec3 color = hdrColor;
//    color *= exposure/(1. + color / exposure);
//    color = pow(color, vec3(1. / gamma));
//    return color;
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
