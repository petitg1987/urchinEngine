#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
} positioningData;

layout(std140, set = 0, binding = 1) uniform Visual {
    float wireframeLineWidth;
} visual;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexData;
invariant gl_Position;

void main() {
    vec4 pointClipSpace = positioningData.mProjectionView * vec4(vertexPosition, 1.0);
    vec2 pointNDC = vec2(pointClipSpace.x / pointClipSpace.w, pointClipSpace.y / pointClipSpace.w);

    vec4 oppostivePointClipSpace = positioningData.mProjectionView * vec4(vertexData.xyz, 1.0);
    vec2 oppostivePointNDC = vec2(oppostivePointClipSpace.x / oppostivePointClipSpace.w, oppostivePointClipSpace.y / oppostivePointClipSpace.w);

    vec2 lineDirection = oppostivePointNDC - pointNDC;
    vec2 perpendicular = normalize(vec2(-lineDirection.y, lineDirection.x));

    float pointSide = vertexData.w;
    vec2 offset = perpendicular * visual.wireframeLineWidth * pointSide;
    offset *= pointClipSpace.w; //keep same line width no matter the projection depth

    pointClipSpace += vec4(offset.x, offset.y, 0.0, 0.0);
    gl_Position = pointClipSpace;
}
