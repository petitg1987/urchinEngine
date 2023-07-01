#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Matrices {
    mat4 mProjectionView;
} matrices;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 texCoord;

layout(location = 0) out vec3 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    gl_Position = matrices.mProjectionView * vec4(vertexPosition, 1.0);
}
