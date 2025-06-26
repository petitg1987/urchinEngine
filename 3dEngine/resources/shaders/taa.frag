#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform GenericData {
    float historyTexIndex;
} data;
layout(binding = 1) uniform sampler2D inputTex;
layout(binding = 2) uniform sampler2D outputOrHistoryTex1;
layout(binding = 3) uniform sampler2D outputOrHistoryTex2;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

void main() {
    if (data.historyTexIndex == 0) {
        fragColor2 = texture(inputTex, texCoordinates);
    } else {
        fragColor1 = texture(inputTex, texCoordinates);
    }
}
