#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform samplerCube diffuseTexture;

layout(location = 0) in vec3 texCoordinates;

layout(location = 0) out vec4 fragDiffuseAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;
layout(location = 2) out vec2 fragPbr;

void main() {
    //diffuse and emissive
    fragDiffuseAndEmissive = vec4(texture(diffuseTexture, texCoordinates).rgb, 0.0);

    //ambient factor
    fragNormalAndAmbient.a = 1.0; //no lighting
}
