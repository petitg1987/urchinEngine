#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseProjectionView;
    mat4 mPreviousProjectionView;
} positioningData;
layout(binding = 1) uniform sampler2D depthTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    float depthValue = texture(depthTex, texCoordinates).r;

    vec4 currentPosNdc = vec4(texCoordinates.s * 2.0 - 1.0, texCoordinates.t * 2.0 - 1.0, depthValue, 1.0);

    vec4 worldPosition = positioningData.mInverseProjectionView * currentPosNdc;
    worldPosition /= worldPosition.w;
    vec4 previousPosNdc = positioningData.mPreviousProjectionView * worldPosition;
    previousPosNdc.xyz /= previousPosNdc.w;

    //NDC are from -1.0 to 1.0 (distance: 2.0).
    //However, we want a velocity of x=1.0 represents a full move from left to right of the screen.
    //Therefore, we multiply by 0.5.
    vec2 velocity = (currentPosNdc.xy - previousPosNdc.xy) * 0.5;
    //TODO apply jitter ?

    fragColor = velocity;
}
