#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
} positioningData;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 oppositePoint;
layout(location = 2) in vec2 pointSide;
invariant gl_Position;

void main() {
    vec4 pointClipSpace = positioningData.mProjectionView * vec4(vertexPosition, 1.0);
    vec2 pointNDC = vec2(pointClipSpace.x / pointClipSpace.w, pointClipSpace.y / pointClipSpace.w);

    vec4 oppostivePointClipSpace = positioningData.mProjectionView * vec4(oppositePoint, 1.0);
    vec2 oppostivePointNDC = vec2(oppostivePointClipSpace.x / oppostivePointClipSpace.w, oppostivePointClipSpace.y / oppostivePointClipSpace.w);

    vec2 lineDirection = oppostivePointNDC - pointNDC;
    vec2 perpendicular = normalize(vec2(-lineDirection.y, lineDirection.x));

    const float lineWidth = 0.05;
    vec2 offset = perpendicular * lineWidth * pointSide.x;

    pointClipSpace += vec4(offset.x, offset.y, 0.0, 0.0);
    gl_Position = pointClipSpace;
}
