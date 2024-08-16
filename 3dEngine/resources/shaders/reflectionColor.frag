#version 460
#extension GL_ARB_separate_shader_objects : enable

//global
layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseProjection;
} positioningData;

layout(binding = 1) uniform sampler2D depthTex;
layout(binding = 2) uniform sampler2D normalAndAmbientTex; //normal XYZ (3 * 8 bits) + ambient factor (8 bits)
layout(binding = 3) uniform sampler2D materialTex; //roughness (8 bits) + metalness (8 bits)
layout(binding = 4) uniform sampler2D illuminatedTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec4 fetchViewSpacePosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0 - 1.0,
        texCoord.t * 2.0 - 1.0,
        depthValue,
        1.0
    );
    vec4 worldPosition = positioningData.mInverseProjection * texPosition;
    worldPosition /= worldPosition.w;
    return worldPosition;
}

void main() {
    float depthValue = texture(depthTex, texCoordinates).r;
    vec4 viewSpacePosition = fetchViewSpacePosition(texCoordinates, depthValue);

    vec3 color = texture(illuminatedTex, texCoordinates).rgb;
    fragColor = vec4(color, 1.0);
}
