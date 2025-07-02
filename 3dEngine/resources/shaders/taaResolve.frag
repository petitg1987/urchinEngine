#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"

layout(binding = 0) uniform sampler2D sceneTex;
layout(binding = 1) uniform sampler2D velocityTex;
layout(binding = 2) uniform sampler2D historyTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

//This method decreases the ghosting effect.
//A value sourced from the history texture that diverges greatly from the scene texture should be discarded.
//As discarding is complex, a clamping is applied instead.
vec3 applyColorClamping(vec3 historyColor) {
    vec2 sceneSize = textureSize(sceneTex, 0);
    vec3 minColor = vec3(9999.0, 9999.0, 9999.0);
    vec3 maxColor = vec3(-9999.0, -9999.0, -9999.0);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 texPosition = texCoordinates + vec2(x / sceneSize.x, y / sceneSize.y);
            texPosition = clamp(texPosition, vec2(0.0, 0.0), vec2(1.0, 1.0));
            vec3 color = texture(sceneTex, texPosition).rgb;
            minColor = min(minColor, color);
            maxColor = max(maxColor, color);
        }
    }

    return clamp(historyColor, minColor, maxColor);
}

void main() {
    vec2 velocity = texture(velocityTex, texCoordinates).xy;
    vec2 prevousPixelPos = texCoordinates - velocity;

    vec3 currentColor = texture(sceneTex, texCoordinates).xyz;
    vec3 historyColor = texture(historyTex, prevousPixelPos).xyz;
    historyColor = applyColorClamping(historyColor);
    historyColor = reduceColorBanding(historyColor, 0.004);

    float historyWeight = 0.9;
    vec3 color = currentColor * (1.0 - historyWeight) + historyColor * historyWeight;

    fragColor = vec4(color, 1.0);
}
