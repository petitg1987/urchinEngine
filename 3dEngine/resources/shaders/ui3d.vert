#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Projection {
    mat4 matrix;
} projection;
layout(std140, set = 0, binding = 1) uniform PositioningData {
    mat4 mViewModel;
    mat4 mNormal;
    ivec2 translate;
} postioningData;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
layout(location = 1) out vec3 normal;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    normal = normalize(mat3(postioningData.mNormal) * vec3(0.0, 0.0, 1.0)); //TODO confirm it works once transform are fixed

    vec4 position = projection.matrix * (postioningData.mViewModel * vec4(vertexPosition + postioningData.translate, 0.0, 1.0));
    gl_Position = position;
}
