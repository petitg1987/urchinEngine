#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Tex {
    vec2 texelSize;
} tex;
layout(binding = 1) uniform sampler2D stepTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

vec3 upSampleTent9Fetch() {
    vec3 topLeft = texture(stepTexture, texCoordinates + tex.texelSize * vec2(-1.0, -1.0)).rgb;
    vec3 topMiddle = texture(stepTexture, texCoordinates + tex.texelSize * vec2(0.0, -1.0)).rgb * 2.0 /* weight */;
    vec3 topRight = texture(stepTexture, texCoordinates + tex.texelSize * vec2(1.0, -1.0)).rgb;

    vec3 middleLeft = texture(stepTexture, texCoordinates + tex.texelSize * vec2(-1.0, 0.0)).rgb * 2.0 /* weight */;
    vec3 center = texture(stepTexture, texCoordinates + tex.texelSize).rgb * 4.0 /* weight */;
    vec3 middleRight = texture(stepTexture, texCoordinates + tex.texelSize * vec2(1.0, 0.0)).rgb * 2.0 /* weight */;

    vec3 bottomLeft = texture(stepTexture, texCoordinates + tex.texelSize * vec2(-1.0, 1.0)).rgb;
    vec3 bottomMiddle = texture(stepTexture, texCoordinates + tex.texelSize * vec2(0.0, 1.0)).rgb * 2.0 /* weight */;
    vec3 bottomRight = texture(stepTexture, texCoordinates + tex.texelSize * vec2(1.0, 1.0)).rgb;

    return (1.0 / 16.0) * (topLeft + topMiddle + topRight + middleLeft + center + middleRight + bottomLeft + bottomMiddle + bottomRight);
}

void main() {
    vec3 texValue = upSampleTent9Fetch();
    fragColor = vec4(texValue, 1.0);
}
