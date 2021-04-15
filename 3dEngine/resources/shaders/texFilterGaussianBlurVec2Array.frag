#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 1) const uint IS_VERTICAL_BLUR = 1;
layout(constant_id = 2) const uint NB_TEXTURE_FETCH = 9; //must be equals to GaussianBlurFilter::TEXTURE_FETCH_SHADER_LIMIT

layout(std140, set = 0, binding = 1) uniform OffsetsBlurData {
    float offsets[NB_TEXTURE_FETCH];
} offsetsBlurData;
layout(std140, set = 0, binding = 2) uniform WeightsBlurData {
    float weights[NB_TEXTURE_FETCH];
} weightsBlurData;

layout(binding = 20) uniform sampler2DArray tex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    fragColor = vec2(0.0, 0.0);

    for (int i = 0; i < NB_TEXTURE_FETCH; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR != 0) ? vec2(0.0, offsetsBlurData.offsets[i]) : vec2(offsetsBlurData.offsets[i], 0.0);
        fragColor += weightsBlurData.weights[i] * texture(tex, vec3(texCoordinates + uvOffset, gl_Layer)).rg;
    }
}