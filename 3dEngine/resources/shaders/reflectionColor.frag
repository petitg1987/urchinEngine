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
    float skipPixelCount = 3.0; //TODO (remove comment): named resolution in tuto
    float thickness   = 0.5;

    vec2 sceneSize = textureSize(depthTex, 0);
    float depthValue = texture(depthTex, texCoordinates).r;
    vec3 normalWorlsSpace = normalize(texture(normalAndAmbientTex, texCoordinates).xyz * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
    vec3 normalViewSpace = normalize(mat3(positioningData.mView) * normalWorlsSpace);
    vec4 viewSpacePosition = fetchViewSpacePosition(texCoordinates, depthValue); //TODO (remove comment): named positionFrom in tuto

    vec3 cameraToPositionVec = normalize(viewSpacePosition.xyz);
    vec3 pivot = normalize(reflect(cameraToPositionVec, normalViewSpace));
    vec4 startViewSpacePosition = viewSpacePosition; //TODO (remove comment): named startView
    vec4 endViewSpacePosition = vec4(viewSpacePosition.xyz + (pivot * maxDistance), 1.0); //TODO (remove comment): named endView

    vec2 startFrag = texCoordinates * sceneSize; //=computeFragPosition(viewSpacePosition, sceneSize);
    vec2 endFrag = computeFragPosition(endViewSpacePosition, sceneSize);

    //DEBUG: visualize the startFrag and endFrag. Red circle is a startFrag and the center of the screen is the corresponding endFrag.
    /* if (distance(endFrag, sceneSize / 2.0f) < 10.0) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    } */

    float deltaX = endFrag.x - startFrag.x;
    float deltaY = endFrag.y - startFrag.y;
    float useX = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float stepNumber = mix(abs(deltaY), abs(deltaX), useX) / skipPixelCount; //TODO (remove comment): named delta in tuto
    vec2 increment = vec2(deltaX, deltaY) / max(stepNumber, 0.001);

    //first pass
    int hasHit = 0;
    vec2 frag = startFrag;
    for (int i = 0; i < int(stepNumber); ++i) {
        frag += increment;
        vec2 uv = frag / sceneSize;

        float fragDepthValue = texture(depthTex, uv).r;
        vec4 viewSpacePositionTo = fetchViewSpacePosition(uv, fragDepthValue);

        float progressionScreenSpace = mix((frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX); //TODO (remove comment): named search1 in tuto
        progressionScreenSpace = clamp(progressionScreenSpace, 0.0, 1.0);

        //Similar to "mix(startViewSpacePosition.z, endViewSpacePosition.z, progressionScreenSpace)" but with perspective-correct interpolation
        //See https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
        float viewDistance = (startViewSpacePosition.z * endViewSpacePosition.z) / mix(endViewSpacePosition.z, startViewSpacePosition.z, progressionScreenSpace);

        float deltaDepth = viewDistance - viewSpacePositionTo.z;
        if (deltaDepth > 0 && deltaDepth < thickness) {
            hasHit = 1;
            break;
        }
    }

    if (hasHit == 1) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 color = texture(illuminatedTex, texCoordinates).rgb;
    fragColor = vec4(color, 1.0);
}
