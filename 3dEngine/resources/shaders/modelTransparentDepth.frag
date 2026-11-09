#version 460
#extension GL_ARB_separate_shader_objects : enable

//Transparent fragments with an alpha greater or equal to this value are written in the depth buffer.
//It allows post-processes using the depth buffer (e.g. TAA reprojection) to work on the mostly opaque parts of the transparent models (e.g. text on a hologram)
//while the see-through parts (e.g. glass) keep the depth of the models behind them.
const float MIN_ALPHA_DEPTH_WRITE = 0.5;

layout(binding = 5) uniform sampler2D albedoTex;

layout(location = 3) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    float alpha = texture(albedoTex, texCoordinates).a;
    if (alpha < MIN_ALPHA_DEPTH_WRITE) {
        discard;
    }

    fragColor = vec4(0.0, 0.0, 0.0, 0.0); //color is not modified thanks to the blend functions
}
