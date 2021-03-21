#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mProjection; //binding 0
uniform mat4 mView; //binding 1
uniform mat4 mModel; //binding 1
uniform mat3 mNormal; //binding 1

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out vec3 t, b, n;
out vec2 textCoordinates;
invariant gl_Position;

void main() {
    textCoordinates = texCoord;

    t = normalize(mNormal * tangent);
    n = normalize(mNormal * normal);
    b = normalize(cross(n, t));

    gl_Position = mProjection * (mView * (mModel * vec4(vertexPosition, 1.0)));
}
