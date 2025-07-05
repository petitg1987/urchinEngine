#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_lightingFunctions.frag"

layout(constant_id = 0) const float REFLECTION_STRENGTH = 0.0;

layout(binding = 0) uniform sampler2D sceneTex;
layout(binding = 1) uniform sampler2D reflectionColorTex; //reflection color (3*8 bits) + visibility (8 bits)

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 color = texture(sceneTex, texCoordinates).rgb;

    vec4 reflectionColorAndVisibility = texture(reflectionColorTex, texCoordinates).rgba;
    float visibility = reduceColorBanding(reflectionColorAndVisibility.a, 0.05);
    float colorWeight = visibility * REFLECTION_STRENGTH;

    fragColor = vec4(mix(color, reflectionColorAndVisibility.rgb, colorWeight), 1.0);
}
