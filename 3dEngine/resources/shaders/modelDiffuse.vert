#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Projection {
    mat4 matrix;
} projection;
layout(std140, set = 0, binding = 1) uniform PositioningData {
    mat4 mView;
    mat4 mModel;
} postioningData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    gl_Position = projection.matrix * (postioningData.mView * (postioningData.mModel * vec4(vertexPosition, 1.0)));
}
