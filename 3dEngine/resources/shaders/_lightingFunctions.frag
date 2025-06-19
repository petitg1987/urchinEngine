const int LIGHT_FLAG_PRODUCE_SHADOW = 0x01; //must match with Light.h
const int LIGHT_FLAG_PBR_ENABLED = 0x02; //must match with Light.h

struct LightInfo {
    bool isExist;
    int lightFlags;
    int lightType;
    vec3 direction;
    vec3 position;
    float exponentialAttenuation;
    float innerCosAngle;
    float outerCosAngle;
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
        float epsilon = lightInfo.innerCosAngle - lightInfo.outerCosAngle;
        float intensity = clamp((theta - lightInfo.outerCosAngle) / epsilon, 0.0, 1.0);
        lightValues.vertexToLight = vertexToLight / dist;
        lightValues.lightAttenuation = exp(-dist * lightInfo.exponentialAttenuation) * intensity;
    }

    lightValues.NdotL = max(dot(normal, lightValues.vertexToLight), 0.0);

    return lightValues;
}

vec3 reduceColorBanding(vec3 value, float strength) {
    float ditheringNoise = fract(52.9829189 * fract(dot(gl_FragCoord.xy, vec2(0.06711056, 0.00583715)))) - 0.5; //from -0.5 to 0.5
    return value + (ditheringNoise * strength);
}
