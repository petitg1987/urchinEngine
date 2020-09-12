#version 450

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec3 vertexNormal;

uniform mat4 mProjection;
uniform mat4 mView;
uniform vec3 vPosition;

out vec3 normal;
out vec2 textCoordinates;
invariant gl_Position;

void main(){
    textCoordinates = texCoord;

    normal = vertexNormal;

    gl_Position = mProjection * mView * vec4(vPosition + vertexPosition, 1.0);
}
