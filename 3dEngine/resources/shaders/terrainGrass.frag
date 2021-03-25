#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define GRASS_ALPHA_TEST 0

uniform float ambient; //binding 4
layout(binding = 20) uniform sampler2D grassTex;

layout(location = 0) smooth in vec2 vertexTexCoordinates;
layout(location = 1) in vec3 grassNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    vec4 color = texture2D(grassTex, vertexTexCoordinates);

    if (color.a < GRASS_ALPHA_TEST)
        discard;

    fragColor = vec4(color.xyz, 1.0);
    fragNormalAndAmbient = vec4(grassNormal, ambient);
}
