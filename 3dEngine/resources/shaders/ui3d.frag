#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float AMBIENT = 0.4;

layout(std140, set = 0, binding = 2) uniform Alpha {
    float factor;
} alpha;
layout(binding = 3) uniform sampler2D albedoTex;

layout(location = 0) in vec2 texCoordinates;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out vec2 fragPbr;

void main() {
    //albedo and emissive
    vec4 color = texture(albedoTex, texCoordinates);
    if (color.a < 0.05) {
        discard;
    }
    color.a *= alpha.factor;
    fragAlbedoAndEmissive = color; //Write color with alpha to compute the blend on the RGB channels. The alpha channel (=emissive) will stay unchanged thanks to the configured alpha blend functions.

    //ambient factor
    vec3 encodedNormal = (normalize(normal) + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(encodedNormal, AMBIENT);

    //pbr
    fragPbr = vec2(1.0, 0.0);
}
