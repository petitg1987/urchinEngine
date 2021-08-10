#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 1) uniform MeshData {
    mat4 mView;
    mat4 mModel;
    mat4 mNormal;
    float ambientFactor;
} meshData;
layout(binding = 2) uniform sampler2D diffuseTex;
layout(binding = 3) uniform sampler2D normalTex;

layout(location = 0) in vec3 t;
layout(location = 1) in vec3 b;
layout(location = 2) in vec3 n;
layout(location = 3) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    fragColor = vec4(texture(diffuseTex, texCoordinates).rgb, 1.0);

    //normal and ambient factor
    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(vec3(texture(normalTex, texCoordinates)) * 2.0 - 1.0);
    vec3 normal = ((tbnMatrix * texNormal) + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(normal, meshData.ambientFactor);
}
