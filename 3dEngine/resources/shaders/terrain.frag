#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 2) uniform Material {
    vec2 stRepeat;
} material;
layout(std140, set = 0, binding = 3) uniform Lighting {
    float ambient;
} lighting;
layout(binding = 4) uniform sampler2D maskTex;
layout(binding = 5) uniform sampler2D diffuseTex1;
layout(binding = 6) uniform sampler2D diffuseTex2;
layout(binding = 7) uniform sampler2D diffuseTex3;
layout(binding = 8) uniform sampler2D diffuseTex4;

layout(location = 0) in vec2 texCoordinates;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    vec2 maskTexCoordinates = vec2(texCoordinates.x / material.stRepeat.x, texCoordinates.y / material.stRepeat.y);
    vec4 maskValue = texture(maskTex, maskTexCoordinates);

    vec4 diffuseValue1 = texture(diffuseTex1, texCoordinates);
    vec4 diffuseValue2 = texture(diffuseTex2, texCoordinates);
    vec4 diffuseValue3 = texture(diffuseTex3, texCoordinates);
    vec4 diffuseValue4 = texture(diffuseTex4, texCoordinates);

    fragColor = maskValue.r * diffuseValue1
        + maskValue.g * diffuseValue2
        + maskValue.b * diffuseValue3
        + maskValue.a * diffuseValue4;

    //material
    vec3 texNormal = (normal + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(texNormal, lighting.ambient);
}
