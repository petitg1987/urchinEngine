#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mView;
    float sumTimeStep;
} positioningData;
layout(std140, set = 0, binding = 1) uniform WaterProperties {
    vec3 color;
    float waveSpeed;
    float waveStrength;
} waterProperties;
layout(binding = 3) uniform sampler2D normalTex;
layout(binding = 4) uniform sampler2D dudvMap;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragDiffuseAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;

vec3 toGlobalNormal(vec3 localNormal) {
    //Water normal is always vec3(0.0, 1.0, 0.0)
    //So, the normal multiply by the TBN matrix can be simplified by invert Y and Z components of the normal
    //TBN matrix = mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0))

    return vec3(localNormal.x, localNormal.z, localNormal.y);
}

void main() {
    float speed = positioningData.sumTimeStep * waterProperties.waveSpeed;
    vec2 distortedTexCoords = texture(dudvMap, vec2(texCoordinates.x + speed, texCoordinates.y)).rg * 0.1;
    distortedTexCoords = texCoordinates + vec2(distortedTexCoords.x, distortedTexCoords.y * speed);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waterProperties.waveStrength;

    //diffuse and emissive
    fragDiffuseAndEmissive = vec4(waterProperties.color, 0.0);

    //normal and ambient factor
    vec2 normalTexCoordinates = texCoordinates + totalDistortion;
    vec3 localNormal = texture(normalTex, normalTexCoordinates).xyz * 2.0 - 1.0;
    vec3 globalNormal = toGlobalNormal(localNormal);
    fragNormalAndAmbient = vec4((globalNormal + 1.0) / 2.0, 0.3);
}
