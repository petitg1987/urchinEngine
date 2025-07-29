#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Matrices {
    mat4 mProjectionView;
} matrices;
layout(std140, set = 0, binding = 1) uniform PositioningData {
    vec3 vPosition;
} positioningData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 vertexNormal;

layout(location = 0) out vec2 texCoordinates;
layout(location = 1) out vec3 normal;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;
    normal = vertexNormal;

    gl_Position = matrices.mProjectionView * vec4(positioningData.vPosition + vertexPosition, 1.0);
}
