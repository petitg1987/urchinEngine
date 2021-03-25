#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mView; //binding 0
uniform mat4 mProjection; //binding 1
uniform vec3 vPosition; //binding 1

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
layout(location = 1) out vec3 normal;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    normal = vertexNormal;

    gl_Position = mProjection * mView * vec4(vPosition + vertexPosition, 1.0);
}
