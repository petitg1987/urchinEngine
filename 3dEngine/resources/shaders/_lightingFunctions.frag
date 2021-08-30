struct LightInfo {
    bool isExist;
    bool produceShadow;
    bool hasParallelBeams;
    vec3 positionOrDirection;
    float exponentialAttenuation;
    vec3 lightAmbient;
};

float computeLightAttenuation(LightInfo lightInfo, vec3 normal, vec3 worldPosition, inout float NdotL) {
    vec3 vertexToLightNormalized;
    float lightAttenuation;

    if (lightInfo.hasParallelBeams) { //sun light
        vec3 vertexToLight = -lightInfo.positionOrDirection;
        vertexToLightNormalized = normalize(vertexToLight);
        lightAttenuation = 1.0;
    } else { //omnidirectional light
        vec3 vertexToLight = lightInfo.positionOrDirection - worldPosition;
        float dist = length(vertexToLight);
        vertexToLightNormalized = normalize(vertexToLight);
        lightAttenuation = exp(-dist * lightInfo.exponentialAttenuation);
    }

    NdotL = max(dot(normal, vertexToLightNormalized), 0.0);

    return lightAttenuation;
}