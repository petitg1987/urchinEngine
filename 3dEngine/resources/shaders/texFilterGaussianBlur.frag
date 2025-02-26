#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_transformFunctions.frag"

layout(constant_id = 0) const uint IS_VERTICAL_BLUR = 1;
layout(constant_id = 1) const uint BLUR_RADIUS = 1;
layout(constant_id = 2) const float MAX_BLUR_DISTANCE = 10.0;
layout(constant_id = 3) const float CAMERA_NEAR_PLANE = 0.01;
layout(constant_id = 4) const float CAMERA_FAR_PLANE = 100.0;

layout(binding = 0) uniform sampler2D tex;
layout(binding = 1) uniform sampler2D depthTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out float fragColor;

float gaussian(float x, float sigma) {
    const float sqrt2pi = 2.50662827463;
    return exp(-0.5 * (x * x) / (sigma * sigma)) / (sqrt2pi * sigma);
}

float computeBlurWeightedValue(vec2 uvOffset, float i, float linearDepth, inout float totalWeight) {
    float texValue = texture(tex, texCoordinates + uvOffset).r;
    float weight = gaussian(i, float(BLUR_RADIUS));

    totalWeight += weight;
    return texValue * weight;
}

void main() {
    vec2 pixelSize = 1.0 / textureSize(tex, 0);

    float depth = texture(depthTex, texCoordinates).r;
    float linearDepth = linearizeDepth(depth, CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE) * CAMERA_FAR_PLANE;
    float blurRadius = mix(0.0, BLUR_RADIUS, max(0.0, 1.0 - (linearDepth / MAX_BLUR_DISTANCE)));
    int blurRadiusInt = int(ceil(blurRadius));

    float totalWeight = 0.0;
    fragColor = computeBlurWeightedValue(vec2(0.0, 0.0), 0.0, linearDepth, totalWeight);

    vec2 blurDirection = (IS_VERTICAL_BLUR == 1) ? vec2(0.0, 1.0) : vec2 (1.0, 0.0);
    for (int i = 1; i <= blurRadiusInt; ++i) {
        vec2 uvOffset = i * pixelSize * blurDirection;
        fragColor += computeBlurWeightedValue(uvOffset, float(i), linearDepth, totalWeight);
        fragColor += computeBlurWeightedValue(-uvOffset, float(i), linearDepth, totalWeight);
    }
    fragColor /= totalWeight;
}
