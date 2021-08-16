#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Projection {
    mat4 matrix;
} projection;
layout(std140, set = 0, binding = 1) uniform PositioningData {
    mat4 mView;
    mat4 mModel;
} postioningData;
layout(std140, set = 0, binding = 2) uniform MeshData {
    mat4 mNormal;
    float ambientFactor;
} meshData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

layout(location = 0) out vec3 t;
layout(location = 1) out vec3 b;
layout(location = 2) out vec3 n;
layout(location = 3) out vec2 texCoordinates;
layout(location = 4) out vec4 worldPosition;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    t = normalize(mat3(meshData.mNormal) * tangent);
    n = normalize(mat3(meshData.mNormal) * normal);
    b = normalize(cross(n, t));

    worldPosition =  postioningData.mModel * vec4(vertexPosition, 1.0);
    gl_Position = projection.matrix * (postioningData.mView * worldPosition);
}
