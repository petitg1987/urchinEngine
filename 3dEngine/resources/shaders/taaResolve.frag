#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D sceneTex;
layout(binding = 1) uniform sampler2D depthTex;
layout(binding = 2) uniform sampler2D velocityTex;
layout(binding = 3) uniform sampler2D historyTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

//Using 'texCoordinates' instead of this function cause ghosting, especially when FPS are low.
vec2 getVelocityTexCoordinate() {
    vec2 sceneSize = textureSize(sceneTex, 0);
    float closestDepth = 1.0;
    vec2 closestTexPosition = vec2(0.0, 0.0);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec2 texPosition = texCoordinates + vec2(x / sceneSize.x, y / sceneSize.y);
            texPosition = clamp(texPosition, vec2(0.0, 0.0), vec2(1.0, 1.0));
            float depthValue = texture(depthTex, texPosition).x;
            if (depthValue < closestDepth) {
                closestDepth = depthValue;
                closestTexPosition = texPosition;
            }
        }
    }

    return closestTexPosition;
}

void main() {
    vec2 velocityTexCoordinate = getVelocityTexCoordinate();
    vec2 velocity = texture(velocityTex, velocityTexCoordinate).xy;
    vec2 prevousPixelPos = texCoordinates - velocity;

    vec3 currentColor = texture(sceneTex, texCoordinates).xyz;
    vec3 historyColor = texture(historyTex, prevousPixelPos).xyz;
    vec3 color = mix(currentColor, historyColor, 0.9);
    fragColor = vec4(color, 1.0);
}
