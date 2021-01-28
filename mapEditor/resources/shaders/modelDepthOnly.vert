#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertexPosition;

uniform mat4 mView;
uniform mat4 mModel;
uniform mat4 mProjection;

invariant gl_Position;

void main() {
    gl_Position = mProjection * (mView * (mModel * vec4(vertexPosition, 1.0)));
}