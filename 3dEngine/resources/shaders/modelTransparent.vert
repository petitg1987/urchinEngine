#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
} postioningData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

//instance data
layout(location = 4) in mat4 mModel; //use location 4, 5, 6 & 7
layout(location = 8) in mat4 mNormal; //use location 8, 9, 10 & 11

layout(location = 0) out vec3 t;
layout(location = 1) out vec3 b;
layout(location = 2) out vec3 n;
layout(location = 3) out vec2 texCoordinates;
layout(location = 4) out vec4 worldPosition;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    t = normalize(mat3(mNormal) * tangent);
    n = normalize(mat3(mNormal) * normal);
    b = normalize(cross(n, t));

    worldPosition =  mModel * vec4(vertexPosition, 1.0);
    gl_Position = postioningData.mProjectionView * worldPosition;
}
