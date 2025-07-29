#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float AMBIENT = 0.0;

layout(std140, set = 0, binding = 2) uniform Material {
    vec2 stRepeat;
} material;
layout(binding = 3) uniform sampler2D maskTex;
layout(binding = 4) uniform sampler2D albedoTex1;
layout(binding = 5) uniform sampler2D albedoTex2;
layout(binding = 6) uniform sampler2D albedoTex3;

layout(location = 0) in vec2 texCoordinates;
layout(location = 1) in vec3 worldPosition;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out vec2 fragPbr;

void main() {
    //albedo
    vec2 maskTexCoordinates = vec2(texCoordinates.x / material.stRepeat.x, texCoordinates.y / material.stRepeat.y);
    vec4 maskValue = texture(maskTex, maskTexCoordinates);

    vec3 albedoValue1 = texture(albedoTex1, texCoordinates).rgb;
    vec3 albedoValue2 = texture(albedoTex2, texCoordinates).rgb;
    vec3 albedoValue3 = texture(albedoTex3, texCoordinates).rgb;

    fragAlbedoAndEmissive.rgb =
            maskValue.r * albedoValue1 +
            maskValue.g * albedoValue2 +
            maskValue.b * albedoValue3;
    fragAlbedoAndEmissive.a = 0.0;

    //normal and ambient
    vec3 texNormal = (normalize(cross(dFdy(worldPosition), dFdx(worldPosition))) + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(texNormal, AMBIENT);

    //pbr
    fragPbr = vec2(1.0, 0.0);
}
