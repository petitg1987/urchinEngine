#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D diffuseTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 accumulationTexture;
layout(location = 1) out float revealTexture;

void main() {
    vec4 color = texture(diffuseTex, texCoordinates);

    //Weight formula (10) from http://jcgt.org/published/0002/02/09/ paper
    //All formulas can be also found in Castor3D (see GlSlUtils#declareComputeAccumulation)
    float oneMinusDepth = 1.0 - gl_FragCoord.z; //from 1.0 (near plane) to 0.0 (far plane)
    float weight = max(0.01, 3000.0 * pow(oneMinusDepth, 3.0));

    accumulationTexture = vec4(color.rgb * color.a, color.a) * weight;
    revealTexture = color.a;
}
