#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PreFilterTweak {
    float threshold;
} preFilterTweak;
layout(std140, set = 0, binding = 1) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 2) uniform sampler2D inputHdrTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

//TODO use include to avoid duplicate
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
    vec3 hdrColor = downSampleBox13Fetch(inputHdrTexture);

    //See following link for the formula: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/
    //Note that more advanced formula to soft edges is also explained under "Soft Threshold" section.
    float brightness = max(max(hdrColor.x, hdrColor.y), hdrColor.z);
    float contribution = max(0.0, brightness - preFilterTweak.threshold) / max(brightness, 0.00001);
    fragColor = vec4(hdrColor * contribution, 1.0);
}
