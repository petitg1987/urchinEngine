#version 450
#define NEAR_PLANE 0
#define FAR_PLANE 1

//values are replaced at compilation time:
#define OUTPUT_TYPE vec3
#define OFFSETS_TAB 0
#define KERNEL_RADIUS 0
#define BLUR_SHARPNESS 0
#define IS_VERTICAL_BLUR true

uniform sampler2D tex;
uniform sampler2D depthTex;
uniform float cameraPlanes[2];

in vec2 textCoordinates;

layout (location = 0) out OUTPUT_TYPE fragColor;

float linearizeDepth(float depthValue) {
    float unmapDepthValue = depthValue * 2.0 - 1.0;
    return (2.0f * cameraPlanes[NEAR_PLANE]) / (cameraPlanes[FAR_PLANE] + cameraPlanes[NEAR_PLANE] -
            unmapDepthValue * (cameraPlanes[FAR_PLANE] - cameraPlanes[NEAR_PLANE])); //[0.0=nearPlane, 1.0=far plane]
}

float bilateralBlur(vec2 uvOffset, int r, float linearizedDepthCenterValue, inout float totalWeight) {
    float texValue = texture2D(tex, textCoordinates+uvOffset).r;
    float depthValue = texture2D(depthTex, textCoordinates).r;
    float linearizedDepthValue = linearizeDepth(depthValue);

    const float blurSigma = float(KERNEL_RADIUS) * 0.5f;
    const float blurFalloff = 1.0 / (2.0*blurSigma*blurSigma);
    float ddiff = (linearizedDepthValue - linearizedDepthCenterValue) * BLUR_SHARPNESS;
    float weight = exp2(-r*r*blurFalloff - ddiff*ddiff);

    totalWeight += weight;

    return texValue * weight;
}

void main() {
    float centerTexValue = texture2D(tex, textCoordinates).r;
    float depthCenterValue = texture2D(depthTex, textCoordinates).r;
    float linearizedDepthCenterValue = linearizeDepth(depthCenterValue);

    float offsets[] = float[](OFFSETS_TAB);

    fragColor = centerTexValue;
    float totalWeight = 1.0f;

    for (int i = 0; i < KERNEL_RADIUS; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, offsets[i]) : vec2(offsets[i], 0.0);
        fragColor += bilateralBlur(uvOffset, i+1, linearizedDepthCenterValue, totalWeight);
    }

    for (int i = 0; i < KERNEL_RADIUS; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, -offsets[i]) : vec2(-offsets[i], 0.0);
        fragColor += bilateralBlur(uvOffset, i+1, linearizedDepthCenterValue, totalWeight);
    }

    fragColor /= totalWeight;
}