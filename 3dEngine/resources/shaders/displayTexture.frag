#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const bool IS_DEFAULT_VALUE = true;
layout(constant_id = 1) const bool IS_GRAYSCALE_VALUE = false;
layout(constant_id = 2) const bool IS_INVERSE_GRAYSCALE_VALUE = false;

layout(std140, set = 0, binding = 1) uniform RenderingData {
    float minColorRange;
    float maxColorRange;
    int layer; //not used
} renderingData;
layout(binding = 2) uniform sampler2D colorTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec4 diffuse = texture(colorTex, texCoordinates);
    vec3 adjustedDiffuse = (diffuse.rgb - vec3(renderingData.minColorRange, renderingData.minColorRange, renderingData.minColorRange))
            * (1.0 / (renderingData.maxColorRange - renderingData.minColorRange));

    vec4 finalDiffuse;
    if (IS_DEFAULT_VALUE) {
        finalDiffuse = vec4(adjustedDiffuse, diffuse.a);
    }else if (IS_GRAYSCALE_VALUE) {
        finalDiffuse = vec4(adjustedDiffuse.r, adjustedDiffuse.r, adjustedDiffuse.r, diffuse.a);
    }else if (IS_INVERSE_GRAYSCALE_VALUE) {
        finalDiffuse = vec4(1.0 - adjustedDiffuse.r, 1.0 - adjustedDiffuse.r, 1.0 - adjustedDiffuse.r, diffuse.a);
    }

    fragColor = finalDiffuse;
}