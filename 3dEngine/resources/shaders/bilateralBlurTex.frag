#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define OUTPUT_TYPE vec3
#define OFFSETS_TAB 0
#define KERNEL_RADIUS 0
#define BLUR_SHARPNESS 0
#define IS_VERTICAL_BLUR true

uniform float cameraNearPlane; //binding 1
uniform float cameraFarPlane; //binding 1
uniform sampler2D tex; //binding 20
uniform sampler2D depthTex; //binding 21

layout(location = 0) in vec2 texCoordinates;

layout (location = 0) out OUTPUT_TYPE fragColor;

float linearizeDepth(float depthValue) {
    float unmapDepthValue = depthValue * 2.0 - 1.0;
    return (2.0f * cameraNearPlane) / (cameraFarPlane + cameraNearPlane -
            unmapDepthValue * (cameraFarPlane - cameraNearPlane)); //[0.0 = nearPlane, 1.0 = far plane]
}

float bilateralBlur(vec2 uvOffset, int r, float linearizedDepthCenterValue, inout float totalWeight) {
    float texValue = texture2D(tex, texCoordinates+uvOffset).r;
    float depthValue = texture2D(depthTex, texCoordinates).r;
    float linearizedDepthValue = linearizeDepth(depthValue);

    const float blurSigma = float(KERNEL_RADIUS) * 0.5f;
    const float blurFalloff = 1.0 / (2.0*blurSigma*blurSigma);
    float ddiff = (linearizedDepthValue - linearizedDepthCenterValue) * BLUR_SHARPNESS;
    float weight = exp2(-r*r*blurFalloff - ddiff*ddiff);

    totalWeight += weight;

    return texValue * weight;
}

void main() {
    float centerTexValue = texture2D(tex, texCoordinates).r;
    float depthCenterValue = texture2D(depthTex, texCoordinates).r;
    float linearizedDepthCenterValue = linearizeDepth(depthCenterValue);

    float offsets[] = float[](OFFSETS_TAB);

    fragColor = centerTexValue;
    float totalWeight = 1.0f;

    for (int i = 0; i < KERNEL_RADIUS; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, offsets[i]) : vec2(offsets[i], 0.0);
        fragColor += bilateralBlur(uvOffset, i + 1, linearizedDepthCenterValue, totalWeight);
    }

    for (int i = 0; i < KERNEL_RADIUS; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, -offsets[i]) : vec2(-offsets[i], 0.0);
        fragColor += bilateralBlur(uvOffset, i + 1, linearizedDepthCenterValue, totalWeight);
    }

    fragColor /= totalWeight;
}