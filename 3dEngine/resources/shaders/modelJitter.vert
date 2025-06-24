#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
} postioningData;

layout(std140, set = 0, binding = 2) uniform FrameData {
    int frameCount;
} frameData;

layout(std140, set = 0, binding = 3) uniform JitterData {
    mat4 offsetsX;
    mat4 offsetsY;
} jitterData;

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
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    t = normalize(mat3(mNormal) * tangent);
    n = normalize(mat3(mNormal) * normal);
    b = normalize(cross(n, t));

    vec4 position = postioningData.mProjectionView * mModel * vec4(vertexPosition, 1.0);

    int offsetIndex = frameData.frameCount % 16;
    int column = offsetIndex / 4;
    int row = offsetIndex % 4;
    position.xy += vec2(jitterData.offsetsX[column][row], jitterData.offsetsY[column][row]) * position.w;

    gl_Position = position;
}
