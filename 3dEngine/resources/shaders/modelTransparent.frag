#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 3) uniform CameraPlanes {
    float nearPlane;
    float farPlane;
} cameraPlanes;
layout(binding = 4) uniform sampler2D diffuseTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 accumulationTexture;
layout(location = 1) out float revealTexture;

void main() {
    vec4 color = texture(diffuseTex, texCoordinates);

    float depth = gl_FragCoord.z; //from 0.0 (near plane) to 1.0 (far plane).
    float linearizedDepth = (cameraPlanes.nearPlane * cameraPlanes.farPlane) /
            (depth * (cameraPlanes.nearPlane - cameraPlanes.farPlane) + cameraPlanes.farPlane); //linearized depth from near plane value to far plane value

    //Weight formulas from paper: http://jcgt.org/published/0002/02/09/
    //Note: formula (10) not work so well when the camera is close to transparent models {weight = max(0.01, 3000.0 * pow(1.0 - depth, 3.0))}
    float weight = max(0.001, min(3000.0, 10.0 / (0.00001 + pow(abs(linearizedDepth) / 5.0, 2.0) + pow(abs(linearizedDepth) / 200.0, 6.0)))); //formula (7)

    accumulationTexture = vec4(color.rgb * color.a, color.a) * weight;
    revealTexture = color.a;
}
