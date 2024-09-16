#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform ProjectionData {
    mat4 mProjection;
    mat4 mInverseProjection;
} projectionData;

layout(std140, set = 0, binding = 1) uniform PositioningData {
    mat4 mView;
} positioningData;

layout(binding = 2) uniform sampler2D depthTex;
layout(binding = 3) uniform sampler2D normalAndAmbientTex; //normal XYZ (3 * 8 bits) + ambient factor (8 bits)
layout(binding = 4) uniform sampler2D materialTex; //roughness (8 bits) + metalness (8 bits)
layout(binding = 5) uniform sampler2D illuminatedTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec4 fetchViewSpacePosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0 - 1.0,
        texCoord.t * 2.0 - 1.0,
        depthValue,
        1.0
    );
    vec4 viewSpacePosition = projectionData.mInverseProjection * texPosition;
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition;
}

vec2 computeFragPosition(vec4 viewSpacePosition, vec2 sceneSize) {
    vec4 fragValue = projectionData.mProjection * viewSpacePosition;
    fragValue.xyz /= fragValue.w;
    fragValue.xy = fragValue.xy * 0.5 + 0.5;
    fragValue.xy *= sceneSize;
    return fragValue.xy;
}

void main() {
    //TODO const
    float maxDistance = 1.0;

    vec2 sceneSize = textureSize(depthTex, 0);
    float depthValue = texture(depthTex, texCoordinates).r;
    vec3 normalWorlsSpace = normalize(texture(normalAndAmbientTex, texCoordinates).xyz * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
    vec3 normalViewSpace = normalize(mat3(positioningData.mView) * normalWorlsSpace);
    vec4 viewSpacePosition = fetchViewSpacePosition(texCoordinates, depthValue); //TODO (remove comment): named positionFrom in tuto

    vec3 cameraToPositionVec = normalize(viewSpacePosition.xyz);
    vec3 pivot = normalize(reflect(cameraToPositionVec, normalViewSpace));
    vec4 startViewSpacePosition = viewSpacePosition; //TODO (remove comment): named startView
    vec4 endViewSpacePosition = vec4(viewSpacePosition.xyz + (pivot * maxDistance), 1.0); //TODO (remove comment): named endView

    vec2 startFrag = texCoordinates * sceneSize; //=computeFragPosition(startViewSpacePosition, sceneSize);
    vec2 endFrag = computeFragPosition(endViewSpacePosition, sceneSize);

    //DEBUG: visualize the startFrag and endFrag. Red circle is a startFrag and the center of the screen is the corresponding endFrag.
    /* if (distance(endFrag, sceneSize / 2.0f) < 10.0) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    } */

    vec3 color = texture(illuminatedTex, texCoordinates).rgb;
    fragColor = vec4(color, 1.0);
}
