#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
    mat4 mNormal;
} postioningData;

layout(location = 0) in vec3 vertexPosition;

//instance data
layout(location = 1) in mat4 mModel; //use location 1, 2, 3 & 4

invariant gl_Position;

void main() {
    gl_Position = postioningData.mProjectionView * (mModel * vec4(vertexPosition, 1.0));
}
