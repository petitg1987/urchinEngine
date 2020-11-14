#version 450

//values are replaced at compilation time:
#define GRASS_ALPHA_TEST 0

smooth in vec2 vertexTextCoordinates;
in vec3 grassNormal;

uniform sampler2D grassTex;
uniform float ambient;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main() {
    vec4 color = texture2D(grassTex, vertexTextCoordinates);

    if (color.a < GRASS_ALPHA_TEST)
        discard;

    fragColor = vec4(color.xyz, 1.0);
    fragNormalAndAmbient = vec4(grassNormal, ambient);
}
