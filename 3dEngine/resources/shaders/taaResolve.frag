#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"

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

// Samples a texture with Catmull-Rom filtering, using 9 texture fetches instead of 16.
// See http://vec3.ca/bicubic-filtering-in-fewer-taps/ for more details
vec3 sample_texture_catmull_rom(sampler2D tex, vec2 uv) {
    // We're going to sample a a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    vec2 resolution = textureSize(tex, 0);
    vec2 sample_position = uv * resolution;
    vec2 tex_pos_1 = floor(sample_position - 0.5f) + 0.5f;

    // Compute the fractional offset from our starting texel to our original sample location, which we'll
    // feed into the Catmull-Rom spline function to get our filter weights.
    vec2 f = sample_position - tex_pos_1;

    // Compute the Catmull-Rom weights using the fractional offset that we calculated earlier.
    // These equations are pre-expanded based on our knowledge of where the texels will be located,
    // which lets us avoid having to evaluate a piece-wise function.
    vec2 w0 = f * (-0.5f + f * (1.0f - 0.5f * f));
    vec2 w1 = 1.0f + f * f * (-2.5f + 1.5f * f);
    vec2 w2 = f * (0.5f + f * (2.0f - 1.5f * f));
    vec2 w3 = f * f * (-0.5f + 0.5f * f);

    // Work out weighting factors and sampling offsets that will let us use bilinear filtering to
    // simultaneously evaluate the middle 2 samples from the 4x4 grid.
    vec2 w12 = w1 + w2;
    vec2 offset_12 = w2 / (w1 + w2);

    // Compute the final UV coordinates we'll use for sampling the texture
    vec2 tex_pos_0 = tex_pos_1 - 1;
    vec2 tex_pos_3 = tex_pos_1 + 2;
    vec2 tex_pos_12 = tex_pos_1 + offset_12;

    tex_pos_0 /= resolution;
    tex_pos_3 /= resolution;
    tex_pos_12 /= resolution;

    vec3 result = vec3(0);
    result += textureLod(tex, vec2(tex_pos_0.x, tex_pos_0.y), 0).rgb * w0.x * w0.y;
    result += textureLod(tex, vec2(tex_pos_12.x, tex_pos_0.y), 0).rgb * w12.x * w0.y;
    result += textureLod(tex, vec2(tex_pos_3.x, tex_pos_0.y), 0).rgb * w3.x * w0.y;

    result += textureLod(tex, vec2(tex_pos_0.x, tex_pos_12.y), 0).rgb * w0.x * w12.y;
    result += textureLod(tex, vec2(tex_pos_12.x, tex_pos_12.y), 0).rgb * w12.x * w12.y;
    result += textureLod(tex, vec2(tex_pos_3.x, tex_pos_12.y), 0).rgb * w3.x * w12.y;

    result += textureLod(tex, vec2(tex_pos_0.x, tex_pos_3.y), 0).rgb * w0.x * w3.y;
    result += textureLod(tex, vec2(tex_pos_12.x, tex_pos_3.y), 0).rgb * w12.x * w3.y;
    result += textureLod(tex, vec2(tex_pos_3.x, tex_pos_3.y), 0).rgb * w3.x * w3.y;

    return result;
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

    //Get  history color
    vec3 historyColor = sample_texture_catmull_rom(historyTex, previousTexPos);
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
    //Reduce those bright pixel based on their luminance.
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
