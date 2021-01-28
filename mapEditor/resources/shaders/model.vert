#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

uniform mat4 mProjection;
uniform mat4 mView;
uniform mat4 mModel;
uniform mat3 mNormal;

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
