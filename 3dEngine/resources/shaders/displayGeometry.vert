#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mProjection; //binding 0
uniform mat4 mView; //binding 0

layout(location = 0) in vec3 vertexPosition;

invariant gl_Position;

void main() {
    gl_Position = mProjection * mView * vec4(vertexPosition, 1.0);
}
