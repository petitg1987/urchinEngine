#version 450

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 texCoord;

uniform mat4 mProjection;
uniform mat4 mView;

out vec3 textCoordinates;
invariant gl_Position;

void main() {
    textCoordinates = texCoord;

    gl_Position = mProjection * (mView * vec4(vertexPosition, 1.0));
}
