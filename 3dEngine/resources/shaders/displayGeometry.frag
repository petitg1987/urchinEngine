#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 2) uniform Visual {
    vec3 color;
} visual;

layout(location = 0) out vec4 fragAlbedoAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out uvec4 fragPbrAndMask;

void main() {
    //albedo and emissive
    fragAlbedoAndEmissive = vec4(visual.color, 0.0);

    //ambient factor
    fragNormalAndAmbient.a = 1.0; //no lighting
}
