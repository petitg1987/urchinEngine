#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}
