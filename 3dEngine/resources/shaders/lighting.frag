#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define NUMBER_SHADOW_MAPS 0
#define SHADOW_MAP_BIAS 0
#define MAX_LIGHTS 0
#define MAX_SHADOW_LIGHTS 0
#define OUTPUT_LOCATION 0

//global
uniform mat4 mInverseViewProjection; //binding 0
uniform vec3 viewPosition; //binding 0
uniform bool hasShadow; //binding 1
uniform bool hasAmbientOcclusion; //binding 1

//lighting
struct StructLightInfo {
    bool isExist;
    bool produceShadow;
    bool hasParallelBeams;
    vec3 positionOrDirection;
    float exponentialAttenuation;
    vec3 lightAmbient;
};
uniform StructLightInfo lightsInfo[MAX_LIGHTS]; //binding 2
uniform vec4 globalAmbient; //binding 3

//shadow
uniform mat4 mLightProjectionView[MAX_SHADOW_LIGHTS][NUMBER_SHADOW_MAPS]; //binding 4
uniform float depthSplitDistance[NUMBER_SHADOW_MAPS]; //binding 5

//fog
uniform bool hasFog; //binding 6
uniform float fogDensity; //binding 6
uniform float fogGradient; //binding 6
uniform float fogMaxHeight; //binding 6
uniform vec4 fogColor; //binding 6

//deferred textures
uniform sampler2D depthTex; //binding 20 - depth (32 bits)
uniform sampler2D colorTex; //binding 21 - diffuse RGB (3*8 bits) + EMPTY (8 bits)
uniform sampler2D normalAndAmbientTex; //binding 22 - normal XYZ (3*8 bits) + ambient factor
uniform sampler2D ambientOcclusionTex; //binding 23 - ambient occlusion factor (16 bits)
uniform sampler2DArray shadowMapTex[MAX_SHADOW_LIGHTS]; //binding 24 - shadow maps for each lights

layout(location = 0) in vec2 texCoordinates;

layout(location = OUTPUT_LOCATION) out vec4 fragColor;

vec4 fetchPosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0f - 1.0f,
        texCoord.t * 2.0f - 1.0f,
        depthValue * 2.0f - 1.0f,
        1.0
    );
    vec4 position = mInverseViewProjection * texPosition;
    position /= position.w;
    return position;
}

float linearStep(float min, float max, float v) {
      return clamp((v - min) / (max - min), 0.0f, 1.0f);
}

float computePercentLit(float shadowMapZ, vec2 moments, float NdotL) {
    float tanAcosNdotL = sqrt(1 - NdotL * NdotL) / NdotL; //=tan(acos(NdotL))
    float bias = max(SHADOW_MAP_BIAS * tanAcosNdotL, 0.00001);
    float shadowMapZBias = shadowMapZ - bias;
    float isInHardShadow = float(shadowMapZBias <= moments.x);

    float variance = moments.y - (moments.x*moments.x);
    float d = moments.x - shadowMapZBias;
    float pMax = variance / (variance + d*d);

    pMax = linearStep(0.75f, 1.0f, pMax); //reduce light bleeding

    return max(isInHardShadow, pMax);
}

float computeShadowContribution(int shadowLightIndex, float depthValue, vec4 position, float NdotL) {
    float shadowContribution = 1.0;

    for (int i = 0; i < NUMBER_SHADOW_MAPS; ++i) {
        if (depthValue < depthSplitDistance[i]) {

            vec4 shadowCoord = (((mLightProjectionView[shadowLightIndex][i] * position) / 2.0) + 0.5);

            //model has produceShadow flag to true ?
            if (shadowCoord.s <= 1.0 && shadowCoord.s >= 0.0 && shadowCoord.t <= 1.0 && shadowCoord.t >= 0.0) {
                vec2 moments = texture2DArray(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st, i)).rg;
                shadowContribution = computePercentLit(shadowCoord.z, moments, NdotL);
            }

            break;
        }
    }

    return shadowContribution;
}

vec4 addFog(vec4 baseColor, vec4 position) {
    if (!hasFog || viewPosition.y > fogMaxHeight) {
        return baseColor;
    }

    vec3 lineVector = position.xyz - viewPosition;
    float t = (fogMaxHeight - viewPosition.y) / lineVector.y;
    vec3 correctedPosition = position.xyz;
    if (t > 0.0 && t < 1.0) {
        correctedPosition = viewPosition + (t * lineVector);
    }

    float distance = distance(viewPosition, correctedPosition);
    float visibility = exp(-pow((distance * fogDensity), fogGradient));
    return mix(fogColor, baseColor, visibility);
}

void main() {
    vec4 diffuse = texture(colorTex, texCoordinates);
    vec4 normalAndAmbient = vec4(texture(normalAndAmbientTex, texCoordinates));
    float modelAmbientFactor = normalAndAmbient.a;
    float depthValue = texture(depthTex, texCoordinates).r;
    vec4 position = fetchPosition(texCoordinates, depthValue);

    if (modelAmbientFactor >= 0.99999f) { //no lighting
        fragColor = addFog(diffuse, position);
        return;
    }

    vec3 normal = vec3(normalAndAmbient) * 2.0f - 1.0f;
    vec4 modelAmbient = diffuse * modelAmbientFactor;
    fragColor = globalAmbient;

    if (hasAmbientOcclusion) {
        float ambientOcclusionFactor = texture2D(ambientOcclusionTex, texCoordinates).r;
        fragColor -= vec4(ambientOcclusionFactor, ambientOcclusionFactor, ambientOcclusionFactor, 0.0f);
    }

    for (int lightIndex = 0, shadowLightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
        if (lightsInfo[lightIndex].isExist) {
            vec3 vertexToLightNormalized;
            float lightAttenuation;

            if (lightsInfo[lightIndex].hasParallelBeams) { //sun light
                vec3 vertexToLight = -lightsInfo[lightIndex].positionOrDirection;
                vertexToLightNormalized = normalize(vertexToLight);
                lightAttenuation = 1.0f;
            } else { //omnidirectional light
                vec3 vertexToLight = lightsInfo[lightIndex].positionOrDirection - vec3(position);
                float dist = length(vertexToLight);
                vertexToLightNormalized = normalize(vertexToLight);
                lightAttenuation = exp(-dist * lightsInfo[lightIndex].exponentialAttenuation);
            }

            float NdotL = max(dot(normal, vertexToLightNormalized), 0.0f);
            vec4 ambient = vec4(lightsInfo[lightIndex].lightAmbient, 0.0f) * modelAmbient;

            float percentLit = 1.0;
            if (hasShadow && lightsInfo[lightIndex].produceShadow) {
                percentLit = computeShadowContribution(shadowLightIndex, depthValue, position, NdotL);
                shadowLightIndex++;
            }

            fragColor += lightAttenuation * (percentLit * (diffuse * NdotL) + ambient);
        }else{
            break; //no more light
        }
    }

    fragColor = addFog(fragColor, position);

    //DEBUG: add color to shadow map splits
/*    const float colorValue = 0.25f;
    vec4 splitColors[5] = vec4[](
        vec4(colorValue, 0.0, 0.0, 1.0), vec4(0.0, colorValue, 0.0, 1.0), vec4(0.0, 0.0, colorValue, 1.0),
        vec4(colorValue, 0.0, colorValue, 1.0), vec4(colorValue, colorValue, 0.0, 1.0));
    for (int i = 0; i < NUMBER_SHADOW_MAPS; ++i) {
        if (depthValue < depthSplitDistance[i]) {
            fragColor += splitColors[i % 5];
            break;
        }
    }*/
}
