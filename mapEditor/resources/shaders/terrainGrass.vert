#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 normal;
invariant gl_Position;

void main() {
    normal = vertexNormal;

    gl_Position = vec4(vertexPosition, 1.0);
}
