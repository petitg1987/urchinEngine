#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform vec2 stRepeat; //binding 2
uniform float ambient; //binding 3

uniform sampler2D maskTex; //binding 20
uniform sampler2D diffuseTex1; //binding 21
uniform sampler2D diffuseTex2; //binding 22
uniform sampler2D diffuseTex3; //binding 23
uniform sampler2D diffuseTex4; //binding 24

in vec2 textCoordinates;
in vec3 normal;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    vec2 maskTextCoordinates = vec2(textCoordinates.x / stRepeat.x, textCoordinates.y / stRepeat.y);
    vec4 maskValue = texture2D(maskTex, maskTextCoordinates);

    vec4 diffuseValue1 = texture2D(diffuseTex1, textCoordinates);
    vec4 diffuseValue2 = texture2D(diffuseTex2, textCoordinates);
    vec4 diffuseValue3 = texture2D(diffuseTex3, textCoordinates);
    vec4 diffuseValue4 = texture2D(diffuseTex4, textCoordinates);

    fragColor = maskValue.r * diffuseValue1
        + maskValue.g * diffuseValue2
        + maskValue.b * diffuseValue3
        + maskValue.a * diffuseValue4;

    //material
    vec3 texNormal = (normal + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(texNormal, ambient);
}
