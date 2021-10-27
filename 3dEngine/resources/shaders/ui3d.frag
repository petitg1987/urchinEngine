#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D diffuseTexture;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragDiffuseAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse and emissive
    vec4 color = texture(diffuseTexture, texCoordinates);
    if (color.a < 0.2) { //TODO review..
        discard;
    }
    fragDiffuseAndEmissive = vec4(color.rgb, 0.0);

    //ambient factor
    fragNormalAndAmbient.a = 1.0; //no lighting //TODO review...
}
