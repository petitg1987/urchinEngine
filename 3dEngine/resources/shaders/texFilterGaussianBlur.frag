#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const bool IS_VERTICAL_BLUR = true;
layout(constant_id = 1) const uint NB_TEXTURE_FETCH = 9; //must be equals to GaussianBlurFilter::TEXTURE_FETCH_SHADER_LIMIT

layout(std140, set = 0, binding = 0) uniform OffsetsBlurData {
    float offsets[NB_TEXTURE_FETCH];
} offsetsBlurData;
layout(std140, set = 0, binding = 1) uniform WeightsBlurData {
    float weights[NB_TEXTURE_FETCH];
} weightsBlurData;

layout(binding = 2) uniform sampler2D tex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out float fragColor;

void main() {
    fragColor = 0.0;

    if (IS_VERTICAL_BLUR) {
        for (int i = 0; i < NB_TEXTURE_FETCH; ++i) {
            fragColor += weightsBlurData.weights[i] * texture(tex, vec2(texCoordinates.x, texCoordinates.y + offsetsBlurData.offsets[i])).r;
        }
    } else {
        for (int i = 0; i < NB_TEXTURE_FETCH; ++i) {
            fragColor += weightsBlurData.weights[i] * texture(tex, vec2(texCoordinates.x + offsetsBlurData.offsets[i], texCoordinates.y)).r;
        }
    }
}