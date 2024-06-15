#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
    vec3 cameraPosition;
    float sumTimeStep;
} positioningData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

//instance data
layout(location = 2) in vec3 grassPosition;
layout(location = 3) in vec3 grassNormal;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    normal = grassNormal;
    texCoordinates = texCoord;

    gl_Position = positioningData.mProjectionView * vec4(grassPosition + vertexPosition, 1.0);
}
