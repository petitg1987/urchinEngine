#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const float GRASS_ALPHA_TEST = 0.0;

layout(std140, set = 0, binding = 3) uniform Lighting {
    float ambient;
} lighting;
layout(binding = 4) uniform sampler2D grassTex;

layout(location = 0) in vec2 vertexTexCoordinates;
layout(location = 1) in vec3 grassNormal;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out vec2 fragPbr;

void main() {
    vec4 color = texture(grassTex, vertexTexCoordinates);

    if (color.a < GRASS_ALPHA_TEST)
        discard;

    fragAlbedoAndEmissive = vec4(color.rgb, 0.0);
    fragNormalAndAmbient = vec4(grassNormal, lighting.ambient);

    //pbr
    fragPbr = vec2(1.0, 0.0);
}
