#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform StableMatrices {
    mat4 mNormal;
} stableMatrices;
layout(std140, set = 0, binding = 1) uniform PositioningData {
    mat4 mProjectionViewModel;
} postioningData;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
layout(location = 1) out vec3 normal;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    normal = normalize(mat3(stableMatrices.mNormal) * vec3(0.0, 0.0, 1.0));

    vec4 position = postioningData.mProjectionViewModel * vec4(vertexPosition, 0.0, 1.0);
    gl_Position = position;
}
