#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mView; //binding 0 (not used)
uniform float sumTimeStep; //binding 0

uniform vec3 waterColor; //binding 1
uniform float waveSpeed; //binding 1
uniform float waveStrength; //binding 1

uniform sampler2D normalTex; //binding 20
uniform sampler2D dudvMap; //bindnig 21

in vec2 textCoordinates;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

vec3 toGlobalNormal(vec3 localNormal) {
    //Water normal is always vec3(0.0, 1.0, 0.0)
    //So, the normal multiply by the TBN matrix can be simplified by invert Y and Z components of the normal
    //TBN matrix = mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0))

    return vec3(localNormal.x, localNormal.z, localNormal.y);
}

void main() {
    float speed = sumTimeStep * waveSpeed;
    vec2 distortedTexCoords = texture(dudvMap, vec2(textCoordinates.x + speed, textCoordinates.y)).rg * 0.1;
    distortedTexCoords = textCoordinates + vec2(distortedTexCoords.x, distortedTexCoords.y * speed);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    //diffuse
    fragColor = vec4(waterColor, 0.0);

    //normal and ambient factor
    vec2 normalTextCoordinates = textCoordinates + totalDistortion;
    vec3 localNormal = texture2D(normalTex, normalTextCoordinates).xyz * 2.0 - 1.0;
    vec3 globalNormal = toGlobalNormal(localNormal);
    fragNormalAndAmbient = vec4((globalNormal + 1.0) / 2.0, 0.3);
}
