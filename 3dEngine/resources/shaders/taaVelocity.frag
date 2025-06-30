#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseViewProjection;
} positioningData;
layout(binding = 1) uniform sampler2D depthTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec2 fragColor;

void main() {
    float depthValue = texture(depthTex, texCoordinates).r;

    vec4 currentPosNdc = vec4(texCoordinates.s * 2.0 - 1.0, texCoordinates.t * 2.0 - 1.0, depthValue, 1.0);

    vec4 worldPosition = positioningData.mInverseViewProjection * currentPosNdc;
    worldPosition /= worldPosition.w;

    fragColor = vec2(0.0, 0.0);
}
