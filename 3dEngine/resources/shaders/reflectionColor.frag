#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjection;
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
    //TODO const
    float maxDistance = 8.0;

    vec2 sceneSize = textureSize(depthTex, 0);
    float depthValue = texture(depthTex, texCoordinates).r;
    vec3 normal = normalize(texture(normalAndAmbientTex, texCoordinates).xyz * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
    vec4 viewSpacePosition = fetchViewSpacePosition(texCoordinates, depthValue); //TODO (remove comment): named positionFrom in tuto

    vec3 pivot = normalize(reflect(viewSpacePosition.xyz, normal));
    vec4 startViewSpacePosition = viewSpacePosition; //TODO (remove comment): named startView
    vec4 endViewSpacePosition = vec4(viewSpacePosition.xyz + (pivot * maxDistance), 1.0); //TODO (remove comment): named endView

    vec4 startFrag = positioningData.mProjection * startViewSpacePosition; //TODO is it equals to viewSpacePosition ?
    startFrag.xyz /= startFrag.w;
    startFrag.xy = startFrag.xy * 0.5 + 0.5;
    startFrag.xy *= sceneSize;

    vec4 endFrag = positioningData.mProjection * endViewSpacePosition;
    endFrag.xyz /= endFrag.w;
    endFrag.xy = endFrag.xy * 0.5 + 0.5;
    endFrag.xy *= sceneSize;

    vec3 color = texture(illuminatedTex, texCoordinates).rgb;
    fragColor = vec4(color, 1.0);
}
