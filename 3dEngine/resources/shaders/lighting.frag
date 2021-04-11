#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

//values are replaced at compilation time:
#define NUMBER_SHADOW_MAPS 0
#define SHADOW_MAP_BIAS 0
#define MAX_LIGHTS 0
#define MAX_SHADOW_LIGHTS 0
#define OUTPUT_LOCATION 0

//global
layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseViewProjection;
    vec3 viewPosition;
} positioningData;
layout(std140, set = 0, binding = 1) uniform VisualOption {
    bool hasShadow;
    bool hasAmbientOcclusion;
} visualOption;

//lighting
struct StructLightInfo {
    bool isExist;
    bool produceShadow;
    bool hasParallelBeams;
    vec3 positionOrDirection;
    float exponentialAttenuation;
    vec3 lightAmbient;
};
layout(std140, set = 0, binding = 2) uniform Lights {
    StructLightInfo lightsInfo[MAX_LIGHTS];
} lights;
layout(std140, set = 0, binding = 3) uniform Lighting {
    vec4 globalAmbient;
} lighting;

//shadow
layout(std140, set = 0, binding = 4) uniform ShadowLight {
    mat4 mLightProjectionView[MAX_SHADOW_LIGHTS * NUMBER_SHADOW_MAPS]; //use 1 dim. table because 2 dim. tables are bugged (only in RenderDoc ?)
} shadowLight;
layout(std140, set = 0, binding = 5) uniform ShadowMap {
    float depthSplitDistance[NUMBER_SHADOW_MAPS];
} shadowMap;

//fog
layout(std140, set = 0, binding = 6) uniform Fog {
    bool hasFog;
    float density;
    float gradient;
    float maxHeight;
    vec4 color;
} fog;

//deferred textures
layout(binding = 20) uniform sampler2D depthTex; //depth (32 bits)
layout(binding = 21) uniform sampler2D colorTex; //diffuse RGB (3*8 bits) + EMPTY (8 bits)
layout(binding = 22) uniform sampler2D normalAndAmbientTex; //normal XYZ (3*8 bits) + ambient factor
layout(binding = 23) uniform sampler2D ambientOcclusionTex; //ambient occlusion factor (16 bits)
layout(binding = 24) uniform sampler2DArray shadowMapTex[MAX_SHADOW_LIGHTS]; //shadow maps for each lights

layout(location = 0) in vec2 texCoordinates;

layout(location = OUTPUT_LOCATION) out vec4 fragColor;

vec4 fetchPosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0f - 1.0f,
        texCoord.t * 2.0f - 1.0f,
        depthValue,
        1.0f
    );
    vec4 position = positioningData.mInverseViewProjection * texPosition;
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

    float variance = moments.y - (moments.x * moments.x);
    float d = moments.x - shadowMapZBias;
    float pMax = variance / (variance + d * d);

    pMax = linearStep(0.75f, 1.0f, pMax); //reduce light bleeding

    return max(isInHardShadow, pMax);
}

float computeShadowContribution(int shadowLightIndex, float depthValue, vec4 position, float NdotL) {
    float shadowContribution = 1.0f;

    for (int i = 0; i < NUMBER_SHADOW_MAPS; ++i) {
        if (depthValue < shadowMap.depthSplitDistance[i]) {
            //TODO: fix glitch probably due to new depth range (0.0-1.0) replacing range old range (-1.0-1.0)
            vec4 shadowCoord = shadowLight.mLightProjectionView[shadowLightIndex * MAX_SHADOW_LIGHTS + i] * position;
            shadowCoord.s = (shadowCoord.s / 2.0f) + 0.5f;
            shadowCoord.t = (shadowCoord.t / 2.0f) + 0.5f;

            //model has produceShadow flag to true ?
            if (shadowCoord.s <= 1.0f && shadowCoord.s >= 0.0f && shadowCoord.t <= 1.0f && shadowCoord.t >= 0.0f) {
                vec2 moments = texture(shadowMapTex[shadowLightIndex], vec3(shadowCoord.st, i)).rg;
                shadowContribution = computePercentLit(shadowCoord.z, moments, NdotL);
            }

            break;
        }
    }

    return shadowContribution;
}

vec4 addFog(vec4 baseColor, vec4 position) {
    if (!fog.hasFog || positioningData.viewPosition.y > fog.maxHeight) {
        return baseColor;
    }

    vec3 lineVector = position.xyz - positioningData.viewPosition;
    float t = (fog.maxHeight - positioningData.viewPosition.y) / lineVector.y;
    vec3 correctedPosition = position.xyz;
    if (t > 0.0f && t < 1.0f) {
        correctedPosition = positioningData.viewPosition + (t * lineVector);
    }

    float distance = distance(positioningData.viewPosition, correctedPosition);
    float visibility = exp(-pow((distance * fog.density), fog.gradient));
    return mix(fog.color, baseColor, visibility);
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
    fragColor = lighting.globalAmbient;

    if (visualOption.hasAmbientOcclusion) {
        float ambientOcclusionFactor = texture(ambientOcclusionTex, texCoordinates).r;
        fragColor -= vec4(ambientOcclusionFactor, ambientOcclusionFactor, ambientOcclusionFactor, 0.0f);
    }

    for (int lightIndex = 0, shadowLightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
        if (lights.lightsInfo[lightIndex].isExist) {
            vec3 vertexToLightNormalized;
            float lightAttenuation;

            if (lights.lightsInfo[lightIndex].hasParallelBeams) { //sun light
                vec3 vertexToLight = -lights.lightsInfo[lightIndex].positionOrDirection;
                vertexToLightNormalized = normalize(vertexToLight);
                lightAttenuation = 1.0f;
            } else { //omnidirectional light
                vec3 vertexToLight = lights.lightsInfo[lightIndex].positionOrDirection - vec3(position);
                float dist = length(vertexToLight);
                vertexToLightNormalized = normalize(vertexToLight);
                lightAttenuation = exp(-dist * lights.lightsInfo[lightIndex].exponentialAttenuation);
            }

            float NdotL = max(dot(normal, vertexToLightNormalized), 0.0f);
            vec4 ambient = vec4(lights.lightsInfo[lightIndex].lightAmbient, 0.0f) * modelAmbient;

            float percentLit = 1.0f;
            if (visualOption.hasShadow && lights.lightsInfo[lightIndex].produceShadow) {
                percentLit = computeShadowContribution(shadowLightIndex, depthValue, position, NdotL);
                shadowLightIndex++;
            }

            fragColor += lightAttenuation * (percentLit * (diffuse * NdotL) + ambient);
        } else {
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
        if (depthValue < shadowMap.depthSplitDistance[i]) {
            fragColor += splitColors[i % 5];
            break;
        }
    }*/
}
