#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"
#include "_samplingFunctions.frag"

layout(binding = 0) uniform sampler2D sceneTex;
layout(binding = 1) uniform sampler2D depthTex;
layout(binding = 2) uniform sampler2D velocityTex;
layout(binding = 3) uniform sampler2D historyTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec4 clipAabb(vec3 aabbMin, vec3 aabbMax, vec4 historySample, float averageAlpha) {
    vec3 pClip = 0.5 * (aabbMax + aabbMin);
    vec3 eClip = 0.5 * (aabbMax - aabbMin) + 0.000000001f;

    vec4 vClip = historySample - vec4(pClip, averageAlpha);
    vec3 vUnit = vClip.xyz / eClip;
    vec3 aUnit = abs(vUnit);
    float maUnit = max(aUnit.x, max(aUnit.y, aUnit.z));

    if (maUnit > 1.0) {
        return vec4(pClip, averageAlpha) + vClip / maUnit;
    } else { //point inside aabb
        return historySample;
    }
}

float filterCubic(in float x, in float b, in float c) {
    float y = 0.0;
    float x2 = x * x;
    float x3 = x * x * x;
    if (x < 1.0) {
        y = (12.0 - 9.0 * b - 6.0 * c) * x3 + (-18.0 + 12.0 * b + 6.0 * c) * x2 + (6.0 - 2.0 * b);
    } else if (x <= 2.0) {
        y = (-b - 6.0 * c) * x3 + (6.0 * b + 30.0 * c) * x2 + (-12.0 * b - 48.0 * c) * x + (8.0 * b + 24.0 * c);
    }
    return y / 6.0;
}

float filterMitchell(float value) {
    return filterCubic(value, 1.0 / 3.0, 1.0 / 3.0);
}

void main() {
    vec2 sceneSize = textureSize(sceneTex, 0);

    vec3 sourceSampleTotal = vec3(0.0, 0.0, 0.0);
    float sourceSampleWeight = 0.0;
    vec3 minColor = vec3(9999.0, 9999.0, 9999.0);
    vec3 maxColor = vec3(-9999.0, -9999.0, -9999.0);
    vec3 moment1 = vec3(0.0, 0.0, 0.0);
    vec3 moment2 = vec3(0.0, 0.0, 0.0);
    float closestDepth = 1.0;
    vec2 closestDepthTexCoordinates = vec2(0.0, 0.0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 subTexCoordinates = clamp(texCoordinates + vec2(x / sceneSize.x, y / sceneSize.y), vec2(0.0, 0.0), vec2(1.0, 1.0));
            vec3 subSourceColor = texture(sceneTex, subTexCoordinates).rgb;

            float subSampleDistance = length(vec2(x, y));
            float subSampleWeight = filterMitchell(subSampleDistance);
            sourceSampleTotal += subSourceColor * subSampleWeight;
            sourceSampleWeight += subSampleWeight;

            minColor = min(minColor, subSourceColor);
            maxColor = max(maxColor, subSourceColor);

            moment1 += subSourceColor;
            moment2 += subSourceColor * subSourceColor;

            float currentDepth = texture(depthTex, subTexCoordinates).r;
            if (currentDepth < closestDepth) {
                closestDepth = currentDepth;
                closestDepthTexCoordinates = subTexCoordinates;
            }
        }
    }

    //Get source color
    vec3 sourceColor = sourceSampleTotal / sourceSampleWeight;
    //TODO better ?: vec3 sourceColor = texture(sceneTex, texCoordinates).xyz;

    //The velocity texture is aliased, unlike the history texture.
    //Therefore, we don't use directly 'texCoordinates' but 'closestDepthTexCoordinates' to get better result and avoid to reintroducing edge aliasing.
    vec2 velocity = texture(velocityTex, closestDepthTexCoordinates).xy;
    vec2 previousTexPos = texCoordinates - velocity;
    if (any(lessThan(previousTexPos, vec2(0.0, 0.0))) || any(greaterThan(previousTexPos, vec2(1.0, 1.0)))) {
        fragColor = vec4(sourceColor, 1.0);
        return;
    }

    //Get history color
    vec3 historyColor = sampleCatmullRom(historyTex, previousTexPos);
    //TODO low quality: vec3 historyColor = texture(historyTex, previousTexPos).xyz;

    //A color sourced from the history texture that diverges greatly from the scene texture should be discarded/adjusted:
    //1. Apply clamp on history color
    historyColor = clamp(historyColor, minColor, maxColor);
    //2. Apply variance clip on history color
    float rcpSampleCount = 1.0f / 9.0f;
    float gamma = 2.0f;
    vec3 mu = moment1 * rcpSampleCount;
    vec3 sigma = sqrt(abs((moment2 * rcpSampleCount) - (mu * mu)));
    vec3 aabbMin = mu - gamma * sigma;
    vec3 aabbMax = mu + gamma * sigma;
    historyColor = clipAabb(aabbMin, aabbMax, vec4(historyColor, 1.0), 1.0).rgb;

    //Due to clamping on history color, some bright pixels (so-called fireflies) can appear briefly due to jittering.
    //Reduce those bright pixels based on their luminance.
    vec3 compressedSource = sourceColor / (max(max(sourceColor.r, sourceColor.g), sourceColor.b) + 1.0);
    vec3 compressedHistory = historyColor / (max(max(historyColor.r, historyColor.g), historyColor.b) + 1.0);
    float luminanceSource = luminance(compressedSource);
    float luminanceHistory = luminance(compressedHistory);
    float sourceWeight = 0.05;
    float historyWeight = 1.0 - sourceWeight;
    sourceWeight *= 1.0 / (1.0 + luminanceSource);
    historyWeight *= 1.0 / (1.0 + luminanceHistory);

    vec3 result = (sourceColor * sourceWeight + historyColor * historyWeight) / max(sourceWeight + historyWeight, 0.00001);
    result = reduceColorBanding(result, 0.002);
    fragColor = vec4(result, 1.0);
}
