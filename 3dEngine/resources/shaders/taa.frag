#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform GenericData {
    int outputIndex;
} data;
layout(binding = 1) uniform sampler2D inputTex;
layout(binding = 2) uniform sampler2D historyTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor1;
layout(location = 1) out vec4 fragColor2;

void main() {
    if (data.outputIndex == 0) {
        fragColor1 = texture(inputTex, texCoordinates);
        fragColor2 = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        fragColor2 = texture(inputTex, texCoordinates);
        fragColor1 = vec4(1.0, 1.0, 0.0, 1.0);
    }
}
