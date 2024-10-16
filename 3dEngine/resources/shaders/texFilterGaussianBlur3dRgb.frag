#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_transformFunctions.frag"

layout(constant_id = 0) const uint KERNEL_RADIUS = 9; //must be equals to GaussianBlur3dFilter::KERNEL_RADIUS_SHADER_LIMIT
layout(constant_id = 1) const float BLUR_SHARPNESS = 0.0;

layout(std140, set = 0, binding = 0) uniform CameraPlanes {
    float nearPlane;
    float farPlane;
} cameraPlanes;
layout(std140, set = 0, binding = 1) uniform BlurData {
    vec2 uvOffsets[KERNEL_RADIUS];
} blurData;
layout(binding = 2) uniform sampler2D tex;
layout(binding = 3) uniform sampler2D depthTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec4 computeBlurWeightedValue(vec2 uvOffset, float blurFalloff, float linearizedDepthCenterValue, inout float totalWeight) {
    float depthValue = texture(depthTex, texCoordinates + uvOffset).r;
    float linearizedDepthValue = linearizeDepth(depthValue, cameraPlanes.nearPlane, cameraPlanes.farPlane);
    float zDiff = (linearizedDepthValue - linearizedDepthCenterValue) * BLUR_SHARPNESS;

    float weight = exp(blurFalloff - zDiff * zDiff);
    totalWeight += weight;

    vec4 texValue = texture(tex, texCoordinates + uvOffset).rgba;
    return texValue * weight;
}

void main() {
    const float blurSigma = (float(KERNEL_RADIUS) + 1.0) * 0.5;
    float depthCenterValue = texture(depthTex, texCoordinates).r;
    float linearizedDepthCenterValue = linearizeDepth(depthCenterValue, cameraPlanes.nearPlane, cameraPlanes.farPlane);
    float totalWeight = 1.0;

    fragColor = texture(tex, texCoordinates).rgba;
    for (int i = 1; i <= KERNEL_RADIUS; ++i) {
        float blurFalloff = -float(i * i) / (2.0 * blurSigma * blurSigma);
        fragColor += computeBlurWeightedValue(blurData.uvOffsets[i], blurFalloff, linearizedDepthCenterValue, totalWeight);
        fragColor += computeBlurWeightedValue(-blurData.uvOffsets[i], blurFalloff, linearizedDepthCenterValue, totalWeight);
    }
    fragColor /= totalWeight;
}
