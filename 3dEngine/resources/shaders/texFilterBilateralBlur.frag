#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 1) const bool IS_VERTICAL_BLUR = true;
layout(constant_id = 2) const uint KERNEL_RADIUS = 9; //must be equals to BilateralBlurFilter::KERNEL_RADIUS_SHADER_LIMIT
layout(constant_id = 3) const float BLUR_SHARPNESS = 0.0;

layout(std140, set = 0, binding = 1) uniform CameraPlanes {
    float nearPlane;
    float farPlane;
} cameraPlanes;
layout(std140, set = 0, binding = 2) uniform BlurData {
    float offsets[KERNEL_RADIUS];
} blurData;
layout(binding = 3) uniform sampler2D tex;
layout(binding = 4) uniform sampler2D depthTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out float fragColor;

float linearizeDepth(float depthValue) {
    float unmapDepthValue = depthValue * 2.0 - 1.0;
    return (2.0 * cameraPlanes.nearPlane) / (cameraPlanes.farPlane + cameraPlanes.nearPlane -
            unmapDepthValue * (cameraPlanes.farPlane - cameraPlanes.nearPlane)); //[0.0 = nearPlane, 1.0 = far plane]
}

float bilateralBlur(vec2 uvOffset, int r, float linearizedDepthCenterValue, inout float totalWeight) {
    float texValue = texture(tex, texCoordinates + uvOffset).r;
    float depthValue = texture(depthTex, texCoordinates).r;
    float linearizedDepthValue = linearizeDepth(depthValue);

    const float blurSigma = float(KERNEL_RADIUS) * 0.5;
    const float blurFalloff = 1.0 / (2.0 * blurSigma * blurSigma);
    float ddiff = (linearizedDepthValue - linearizedDepthCenterValue) * BLUR_SHARPNESS;
    float weight = exp2(-r * r * blurFalloff - ddiff * ddiff);

    totalWeight += weight;

    return texValue * weight;
}

void main() {
    float centerTexValue = texture(tex, texCoordinates).r;
    float depthCenterValue = texture(depthTex, texCoordinates).r;
    float linearizedDepthCenterValue = linearizeDepth(depthCenterValue);

    fragColor = centerTexValue;
    float totalWeight = 1.0;

    for (int i = 0; i < KERNEL_RADIUS; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, blurData.offsets[i]) : vec2(blurData.offsets[i], 0.0);
        fragColor += bilateralBlur(uvOffset, i + 1, linearizedDepthCenterValue, totalWeight);
    }

    for (int i = 0; i < KERNEL_RADIUS; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, -blurData.offsets[i]) : vec2(-blurData.offsets[i], 0.0);
        fragColor += bilateralBlur(uvOffset, i + 1, linearizedDepthCenterValue, totalWeight);
    }

    fragColor /= totalWeight;
}