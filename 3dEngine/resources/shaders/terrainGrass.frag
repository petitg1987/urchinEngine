#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float GRASS_ALPHA_TEST = 0.0;

layout(std140, set = 0, binding = 2) uniform Lighting {
    float ambient;
    uint lightMask;
} lighting;
layout(binding = 3) uniform sampler2D grassTex;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 texCoordinates;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out uvec4 fragPbrAndMask;

void main() {
    vec4 color = texture(grassTex, texCoordinates);

    if (color.a < GRASS_ALPHA_TEST) {
        discard;
    }

    fragAlbedoAndEmissive = vec4(color.rgb, 0.0);
    fragNormalAndAmbient = vec4(normal, lighting.ambient);

    //pbr & mask
    fragPbrAndMask.r = 255; //roughness
    fragPbrAndMask.g = 0; //metalness
    fragPbrAndMask.b = lighting.lightMask; //light mask
}
