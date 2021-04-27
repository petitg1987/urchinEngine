#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float POINT_SIZE = 1.0f;

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjection;
    mat4 mView;
} positioningData;

layout(location = 0) in vec3 vertexPosition;
invariant gl_Position;

void main() {
    gl_Position = positioningData.mProjection * positioningData.mView * vec4(vertexPosition, 1.0);
    gl_PointSize = POINT_SIZE;
}
