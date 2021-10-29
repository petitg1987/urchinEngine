#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform StableMatrices {
    mat4 mProjection;
    mat4 mNormal;
} stableMatrices;
layout(std140, set = 0, binding = 1) uniform PositioningData {
    mat4 mViewModel;
} postioningData;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    vec4 position = stableMatrices.mProjection * (postioningData.mViewModel * vec4(vertexPosition, 1.0, 1.0));
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
