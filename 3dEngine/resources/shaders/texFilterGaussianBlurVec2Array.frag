#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define WEIGHTS_TAB 0
#define OFFSETS_TAB 0
#define NB_TEXTURE_FETCH 0
#define IS_VERTICAL_BLUR true

layout(binding = 20) uniform sampler2DArray tex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    fragColor = vec2(0.0, 0.0);

    float weights[] = float[](WEIGHTS_TAB);
    float offsets[] = float[](OFFSETS_TAB);

    for (int i = 0; i < NB_TEXTURE_FETCH; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, offsets[i]) : vec2(offsets[i], 0.0);
        fragColor += weights[i] * texture(tex, vec3(texCoordinates + uvOffset, gl_Layer)).rg;
    }
}