struct LightInfo {
    bool isExist;
    bool produceShadow;
    int lightType;
    vec3 direction;
    vec3 position;
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

    if (lightInfo.lightType == 0) { //sun light
        lightValues.vertexToLight = normalize(-lightInfo.direction);
        lightValues.lightAttenuation = 1.0;
    } else if (lightInfo.lightType == 1) { //omnidirectional light
        vec3 vertexToLight = lightInfo.position - worldPosition;
        float dist = length(vertexToLight);
        lightValues.vertexToLight = vertexToLight / dist;
        lightValues.lightAttenuation = exp(-dist * lightInfo.exponentialAttenuation);
    }

    lightValues.NdotL = max(dot(normal, lightValues.vertexToLight), 0.0);

    return lightValues;
}
