#version 450
#extension GL_EXT_gpu_shader4 : enable

//values are replaced at compilation time:
#define OUTPUT_TYPE vec3
#define WEIGHTS_TAB 0
#define OFFSETS_TAB 0
#define NB_TEXTURE_FETCH 0
#define IS_VERTICAL_BLUR true
#define SOURCE_TEX_COMPONENTS rgb

uniform sampler2DArray tex;

in vec2 vertexTextCoordinates;

layout (location = 0) out OUTPUT_TYPE fragColor;

void main() {
    fragColor = OUTPUT_TYPE(0.0);

    float weights[] = float[](WEIGHTS_TAB);
    float offsets[] = float[](OFFSETS_TAB);

    for (int i = 0; i < NB_TEXTURE_FETCH; ++i) {
        vec2 uvOffset = (IS_VERTICAL_BLUR) ? vec2(0.0, offsets[i]) : vec2(offsets[i], 0.0);
        fragColor += weights[i] * texture2DArray(tex, vec3(vertexTextCoordinates+uvOffset, gl_Layer)).SOURCE_TEX_COMPONENTS;
    }
}