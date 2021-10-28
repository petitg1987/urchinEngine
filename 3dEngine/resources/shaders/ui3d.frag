#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D diffuseTexture;

layout(location = 0) in vec2 texCoordinates;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 fragDiffuseAndEmissive;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse and emissive
    vec4 color = texture(diffuseTexture, texCoordinates);
    if (color.a < 0.1) { //no transparency for UI 3d
        discard; //TODO [FOR_LATER] review
    }
    fragDiffuseAndEmissive = vec4(color.rgb, 0.0);

    //ambient factor
    vec3 encodedNormal = (normalize(normal) + 1.0f) / 2.0;
    fragNormalAndAmbient = vec4(encodedNormal, 0.4); //TODO [FOR_LATER] parameter
}
