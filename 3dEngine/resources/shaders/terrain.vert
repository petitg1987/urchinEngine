#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mView; //binding 0
uniform mat4 mProjection; //binding 1
uniform vec3 vPosition; //binding 1

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 texCoord;

out vec3 normal;
out vec2 textCoordinates;
invariant gl_Position;

void main() {
    textCoordinates = texCoord;

    normal = vertexNormal;

    gl_Position = mProjection * mView * vec4(vPosition + vertexPosition, 1.0);
}
