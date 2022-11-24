struct LightInfo {
    bool isExist;
    bool produceShadow;
    bool hasParallelBeams;
    vec3 positionOrDirection;
    float exponentialAttenuation;
    vec3 lightColor;
};

struct LightValues {
    vec3 vertexToLight;
    float lightAttenuation;
    float NdotL;
};

LightValues computeLightValues(LightInfo lightInfo, vec3 normal, vec3 worldPosition) {
    LightValues lightValues;

    if (lightInfo.hasParallelBeams) { //sun light
        lightValues.vertexToLight = normalize(-lightInfo.positionOrDirection);
        lightValues.lightAttenuation = 1.0;
    } else { //omnidirectional light
        vec3 vertexToLight = lightInfo.positionOrDirection - worldPosition;
        float dist = length(vertexToLight);
        lightValues.vertexToLight = vertexToLight / dist;
        lightValues.lightAttenuation = exp(-dist * lightInfo.exponentialAttenuation);
    }

    lightValues.NdotL = max(dot(normal, lightValues.vertexToLight), 0.0);

    return lightValues;
}
