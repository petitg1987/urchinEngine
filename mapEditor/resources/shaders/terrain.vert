#version 330

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;

uniform mat4 mProjection;
uniform mat4 mView;

out vec3 normal;
invariant gl_Position;

void main(){
    normal = vertexNormal;

	gl_Position = mProjection * mView * vec4(vertexPosition, 1.0);
}
