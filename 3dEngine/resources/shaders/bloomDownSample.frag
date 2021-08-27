#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 1) uniform sampler2D stepTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec3 downSampleBox13Fetch(sampler2D inputTex) {
    //See http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare

    //center square texels
    vec3 centerA = texture(inputTex, texCoordinates + tex.texelSize * vec2(-1.0, -1.0)).rgb;
    vec3 centerB = texture(inputTex, texCoordinates + tex.texelSize * vec2(1.0, -1.0)).rgb;
    vec3 centerC = texture(inputTex, texCoordinates + tex.texelSize * vec2(-1.0, 1.0)).rgb;
    vec3 centerD = texture(inputTex, texCoordinates + tex.texelSize * vec2(1.0, 1.0)).rgb;

    //top texels
    vec3 topA = texture(inputTex, texCoordinates + tex.texelSize * vec2(-2.0, -2.0)).rgb;
    vec3 topB = texture(inputTex, texCoordinates + tex.texelSize * vec2(0.0, -2.0)).rgb;
    vec3 topC = texture(inputTex, texCoordinates + tex.texelSize * vec2(2.0, -2.0)).rgb;

    //middle texels
    vec3 middleA = texture(inputTex, texCoordinates + tex.texelSize * vec2(-2.0, 0.0)).rgb;
    vec3 middleB = texture(inputTex, texCoordinates).rgb;
    vec3 middleC = texture(inputTex, texCoordinates + tex.texelSize * vec2(-2.0, 0.0)).rgb;

    //bottom texels
    vec3 bottomA = texture(inputTex, texCoordinates + tex.texelSize * vec2(-2.0, 2.0)).rgb;
    vec3 bottomB = texture(inputTex, texCoordinates + tex.texelSize * vec2(0.0, 2.0)).rgb;
    vec3 bottomC = texture(inputTex, texCoordinates + tex.texelSize * vec2(2.0, 2.0)).rgb;

    const float centerSquareWeight = (1.0 / 4.0) * 0.5;
    const float cornerSquaresWeight = (1.0 / 4.0) * 0.125;

    vec3 averageValue = centerSquareWeight * (centerA + centerB + centerC + centerD); //center square (red)
    averageValue += cornerSquaresWeight * (topA + topB + middleA + middleB); //top left square (yellow)
    averageValue += cornerSquaresWeight * (topB + topC + middleB + middleC); //top right square (green)
    averageValue += cornerSquaresWeight * (middleA + middleB + bottomA + bottomB); //bottom left square (purple)
    averageValue += cornerSquaresWeight * (middleB + middleC + bottomB + bottomC); //bottom right square (blue)

    return averageValue;
}

void main() {
    vec3 texValue = downSampleBox13Fetch(stepTexture);
    fragColor = vec4(texValue, 1.0);
}
