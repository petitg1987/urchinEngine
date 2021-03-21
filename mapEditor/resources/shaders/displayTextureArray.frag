#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_gpu_shader4 : enable

//values are replaced at compilation time:
#define IS_DEFAULT_VALUE true
#define IS_DEPTH_VALUE false
#define IS_GRAYSCALE_VALUE false
#define IS_INVERSE_GRAYSCALE_VALUE false

uniform float colorIntensity; //binding 1
uniform float cameraNearPlane; //binding 1
uniform float cameraFarPlane; //binding 1
uniform int layer; //binding 1
uniform sampler2DArray colorTex; //binding 20

in vec2 textCoordinates;

out vec4 fragColor;

float linearizeDepth(float depthValue) {
    float unmapDepthValue = depthValue * 2.0 - 1.0;
    return (2.0f * cameraNearPlane) / (cameraFarPlane + cameraNearPlane -
            unmapDepthValue * (cameraFarPlane - cameraNearPlane)); //[0.0 = nearPlane, 1.0 = far plane]
}

void main() {
    vec4 diffuse = texture2DArray(colorTex, vec3(textCoordinates, layer));

    vec4 finalDiffuse;
    if (IS_DEFAULT_VALUE) {
        finalDiffuse = diffuse * colorIntensity;
    }else if (IS_DEPTH_VALUE) {
        float linearizedDepthValue = linearizeDepth(diffuse.x) * colorIntensity;
        finalDiffuse = vec4(linearizedDepthValue, linearizedDepthValue, linearizedDepthValue, diffuse.w);
    }else if (IS_GRAYSCALE_VALUE) {
        float value = diffuse.x * colorIntensity;
        finalDiffuse = vec4(value, value, value, diffuse.w);
    }else if (IS_INVERSE_GRAYSCALE_VALUE) {
        float value = 1.0f - (diffuse.x * colorIntensity);
        finalDiffuse = vec4(value, value, value, diffuse.w);
    }

    fragColor = finalDiffuse;
}