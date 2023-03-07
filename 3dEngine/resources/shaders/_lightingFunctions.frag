struct LightInfo {
    bool isExist;
    bool produceShadow;
    int lightType;
    vec3 direction;
    vec3 position;
    float exponentialAttenuation;
    float innerCutOff;
    float outerCutOff;
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
    } else if (lightInfo.lightType == 2) { //spot light
        vec3 vertexToLight = lightInfo.position - worldPosition;
        float dist = length(vertexToLight);
        float theta = dot(normalize(vertexToLight), -lightInfo.direction);
        float epsilon = lightInfo.innerCutOff - lightInfo.outerCutOff;
        float intensity = clamp((theta - lightInfo.outerCutOff) / epsilon, 0.0, 1.0);
        lightValues.vertexToLight = vertexToLight / dist;
        lightValues.lightAttenuation = exp(-dist * lightInfo.exponentialAttenuation);
        lightValues.lightAttenuation *= intensity;
    }

    lightValues.NdotL = max(dot(normal, lightValues.vertexToLight), 0.0);

    return lightValues;
}
