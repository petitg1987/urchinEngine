#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 1) const bool IS_VERTICAL_BLUR = true;
layout(constant_id = 2) const uint NB_TEXTURE_FETCH = 0;
layout(constant_id = 3) const float[] WEIGHTS = float[](0.0f);
layout(constant_id = 4) const float[] OFFSETS = float[](0.0f);

layout(binding = 20) uniform sampler2DArray tex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    fragColor = vec2(0.0, 0.0);

    for (int i = 0; i < NB_TEXTURE_FETCH; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, OFFSETS[i]) : vec2(OFFSETS[i], 0.0);
        fragColor += WEIGHTS[i] * texture(tex, vec3(texCoordinates + uvOffset, gl_Layer)).rg;
    }
}