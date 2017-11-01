#version 330

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec3 vertexNormal;

uniform mat4 mProjection;
uniform mat4 mView;
uniform mat4 mModel;

out vec3 normal;
out vec2 textCoordinates;
invariant gl_Position;

void main(){
    textCoordinates = texCoord;

    normal = vertexNormal;

	gl_Position = mProjection * mView * mModel * vec4(vertexPosition, 1.0);
}
