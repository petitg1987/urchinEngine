#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "_samplingFunctions.frag"

layout(constant_id = 0) const bool QUALITY_TEXTURE_FETCH = true;
layout(constant_id = 1) const float SAMPLE_SCALE = 1.0;

layout(std140, set = 0, binding = 0) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 1) uniform sampler2D stepTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 texValue;
    if (QUALITY_TEXTURE_FETCH) {
        texValue = upSample9Fetch(stepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    } else {
        texValue = upSample4Fetch(stepTexture, texCoordinates, tex.texelSize, SAMPLE_SCALE);
    }

    fragColor = vec4(texValue, 1.0);
}
