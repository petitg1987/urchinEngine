#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform samplerCube diffuseTexture;

layout(location = 0) in vec3 texCoordinates;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    fragColor = texture(diffuseTexture, texCoordinates);

    //material
    fragNormalAndAmbient.a = 1.0; //no lighting
}
