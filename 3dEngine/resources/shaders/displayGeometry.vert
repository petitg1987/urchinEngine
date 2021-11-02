#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
} positioningData;

layout(location = 0) in vec3 vertexPosition;
invariant gl_Position;

void main() {
    gl_Position = positioningData.mProjectionView * vec4(vertexPosition, 1.0);
}
