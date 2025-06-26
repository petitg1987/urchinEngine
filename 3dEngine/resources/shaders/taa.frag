#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Scene {
    vec2 invSceneSize;
} scene;
layout(binding = 1) uniform sampler2D tex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = texture(tex, texCoordinates);
}
