#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Projection {
    mat4 mProjection;
} projection;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    vec4 position = projection.mProjection * vec4(vertexPosition, 1.0, 1.0);
    position /= position.z;

    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}