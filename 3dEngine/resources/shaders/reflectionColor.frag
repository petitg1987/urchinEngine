#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float MAX_DISTANCE = 0.0;
layout(constant_id = 1) const float HIT_THRESHOLD = 0.0;
layout(constant_id = 2) const float SKIP_PIXEL_COUNT_ON_FIRST_PASS = 0.0;
layout(constant_id = 3) const uint NUM_STEPS_ON_SECOND_PASS = 0;

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

vec4 fetchViewSpacePosition(vec2 texCoord) {
    float depthValue = texture(depthTex, texCoord).r;
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
    fragValue.xy /= fragValue.w;
    fragValue.xy = fragValue.xy * 0.5 + 0.5;
    fragValue.xy *= sceneSize;
    return fragValue.xy;
}

void main() {
    const float MAX_MATERIAL_ROUGHNESS = 0.9;
    float materialRoughness = texture(materialTex, texCoordinates).r;
    if (materialRoughness > MAX_MATERIAL_ROUGHNESS) {
        fragColor = vec4(0.5, 0.5, 0.5, 0.0);
        return;
    }

    vec2 sceneSize = textureSize(depthTex, 0);
    vec3 normalWorlsSpace = normalize(texture(normalAndAmbientTex, texCoordinates).xyz * 2.0 - 1.0); //normalize is required (for good specular) because normal is stored in 3 * 8 bits only
    vec3 normalViewSpace = normalize(mat3(positioningData.mView) * normalWorlsSpace);
    vec4 viewSpacePosition = fetchViewSpacePosition(texCoordinates);

    vec3 cameraToPositionVec = normalize(viewSpacePosition.xyz);
    vec3 pivot = normalize(reflect(cameraToPositionVec, normalViewSpace));
    vec4 startViewSpacePosition = viewSpacePosition;
    vec4 endViewSpacePosition = vec4(viewSpacePosition.xyz + (pivot * MAX_DISTANCE), 1.0);

    vec2 startFrag = texCoordinates * sceneSize; //=computeFragPosition(viewSpacePosition, sceneSize);
    vec2 endFrag = computeFragPosition(endViewSpacePosition, sceneSize);

    //DEBUG: visualize the startFrag and endFrag. Red circle is a startFrag and the center of the screen is the corresponding endFrag.
    /* if (distance(endFrag, sceneSize / 2.0f) < 10.0) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    } */

    vec2 frag = startFrag;
    vec2 fragUv = texCoordinates;

    float hitBoundary1 = 0.0;
    float hitBoundary2 = 0.0;

    float depth = HIT_THRESHOLD;
    vec4 viewSpacePositionTo = viewSpacePosition;

    //FIRST PASS
    int firstPassHasHit = 0;
    float deltaX = endFrag.x - startFrag.x;
    float deltaY = endFrag.y - startFrag.y;
    float useX = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float firstPassSteps = mix(abs(deltaY), abs(deltaX), useX) / SKIP_PIXEL_COUNT_ON_FIRST_PASS;
    vec2 increment = vec2(deltaX, deltaY) / max(firstPassSteps, 0.001);

    for (int i = 0; i < int(firstPassSteps); ++i) {
        frag += increment;
        fragUv = frag / sceneSize;

        if (fragUv.x >= 0.0 && fragUv.x <= 1.0 && fragUv.y >= 0.0 && fragUv.y <= 1.0) {
            float progressionScreenSpace = mix((frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);
            progressionScreenSpace = clamp(progressionScreenSpace, 0.0, 1.0);

            //Similar to "mix(startViewSpacePosition.z, endViewSpacePosition.z, progressionScreenSpace)" but with perspective-correct interpolation
            //See https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
            float viewDistance = (startViewSpacePosition.z * endViewSpacePosition.z) / mix(endViewSpacePosition.z, startViewSpacePosition.z, progressionScreenSpace);
            viewSpacePositionTo = fetchViewSpacePosition(fragUv);
            float depth = viewSpacePositionTo.z - viewDistance;

            if (depth >= 0.0 /* hit found */ && depth < HIT_THRESHOLD /* hit is close to viewSpacePositionTo */) {
                firstPassHasHit = 1;
                hitBoundary1 = progressionScreenSpace;
                break;
            } else {
                hitBoundary2 = hitBoundary1;
            }
        } else {
            break;
        }
    }

    //DEBUG: visualize the hit in red
    /* if (firstPassHasHit == 1) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    } */

    //SECOND PASS
    int secondPassHasHit = 0;
    uint secondPassSteps = NUM_STEPS_ON_SECOND_PASS * firstPassHasHit;
    for (uint i = 0; i < secondPassSteps; ++i) {
        float hitBoundaryMiddle = (hitBoundary1 + hitBoundary2) / 2.0;
        frag = mix(startFrag.xy, endFrag.xy, hitBoundaryMiddle);
        fragUv = frag / sceneSize;

        if (fragUv.x >= 0.0 && fragUv.x <= 1.0 && fragUv.y >= 0.0 && fragUv.y <= 1.0) {
            float viewDistance = (startViewSpacePosition.z * endViewSpacePosition.z) / mix(endViewSpacePosition.z, startViewSpacePosition.z, hitBoundaryMiddle);
            viewSpacePositionTo = fetchViewSpacePosition(fragUv);
            depth = viewSpacePositionTo.z - viewDistance;

            if (depth >= -0.01 /* hit found */ && depth < HIT_THRESHOLD /* hit is close to viewSpacePositionTo */) {
                secondPassHasHit = 1;
                hitBoundary1 = hitBoundaryMiddle;
            } else {
                hitBoundary2 = hitBoundaryMiddle;
            }
        }
    }

    //DEBUG: visualize the hit in red
    /* if (secondPassHasHit == 1) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    } */

    float edgeThreshold = 0.2;
    float visibility = secondPassHasHit
        * (1.0 - max(dot(-cameraToPositionVec, pivot), 0.0)) //Eliminate reflection rays pointing to camera and probably hitting something behind the camera
        * (1.0 - clamp(depth / HIT_THRESHOLD, 0.0, 1.0)) //Fade out the reflection point when the exact hit point is not found
        * (1.0 - clamp(length(viewSpacePositionTo - viewSpacePosition) / MAX_DISTANCE, 0.0, 1.0)) //Fade out the reflection based on how far way the reflected point is from the initial starting point
        * smoothstep(0.0, edgeThreshold, fragUv.x) * (1.0 - smoothstep(1.0 - edgeThreshold, 1.0, fragUv.x)) //Fade out on screen edge X
        * smoothstep(0.0, edgeThreshold, fragUv.y) * (1.0 - smoothstep(1.0 - edgeThreshold, 1.0, fragUv.y)) //Fade out on screen edge Y
        * (MAX_MATERIAL_ROUGHNESS - materialRoughness);

    vec3 reflectionColor = texture(illuminatedTex, fragUv).rgb;
    fragColor = vec4(reflectionColor, visibility);
}
