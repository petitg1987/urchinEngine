#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const bool IS_DEFAULT_VALUE = true;
layout(constant_id = 1) const bool IS_DEPTH_VALUE = false;
layout(constant_id = 2) const bool IS_GRAYSCALE_VALUE = false;
layout(constant_id = 3) const bool IS_INVERSE_GRAYSCALE_VALUE = false;

layout(std140, set = 0, binding = 1) uniform RenderingData {
    float colorIntensity;
    float cameraNearPlane;
    float cameraFarPlane;
    int layer;
} renderingData;
layout(binding = 2) uniform sampler2DArray colorTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

float linearizeDepth(float depthValue) {
    float unmapDepthValue = depthValue * 2.0 - 1.0;
    return (2.0f * renderingData.cameraNearPlane) / (renderingData.cameraFarPlane + renderingData.cameraNearPlane -
            unmapDepthValue * (renderingData.cameraFarPlane - renderingData.cameraNearPlane)); //[0.0 = nearPlane, 1.0 = far plane]
}

void main() {
    vec4 diffuse = texture(colorTex, vec3(texCoordinates, renderingData.layer));

    vec4 finalDiffuse;
    if (IS_DEFAULT_VALUE) {
        finalDiffuse = diffuse * renderingData.colorIntensity;
    }else if (IS_DEPTH_VALUE) {
        float linearizedDepthValue = linearizeDepth(diffuse.x) * renderingData.colorIntensity;
        finalDiffuse = vec4(linearizedDepthValue, linearizedDepthValue, linearizedDepthValue, diffuse.w);
    }else if (IS_GRAYSCALE_VALUE) {
        float value = diffuse.x * renderingData.colorIntensity;
        finalDiffuse = vec4(value, value, value, diffuse.w);
    }else if (IS_INVERSE_GRAYSCALE_VALUE) {
        float value = 1.0f - (diffuse.x * renderingData.colorIntensity);
        finalDiffuse = vec4(value, value, value, diffuse.w);
    }

    fragColor = finalDiffuse;
}