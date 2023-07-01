#version 460
#extension GL_ARB_separate_shader_objects : enable

const uint LIGHTS_SHADER_LIMIT = 15; //must be equals to LightManager::LIGHTS_SHADER_SHADER_LIMIT
layout(constant_id = 0) const uint NUMBER_SHADOW_MAPS = 7; //must be equals to ShadowManager::SHADOW_MAPS_SHADER_LIMIT

layout(std140, set = 0, binding = 2) uniform ShadowData {
    mat4 matrices[NUMBER_SHADOW_MAPS];
} shadowData;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3 * LIGHTS_SHADER_LIMIT) out;

void main() {
    for (int layerIndex = 0; layerIndex < NUMBER_SHADOW_MAPS; layerIndex++) {
        gl_Layer = layerIndex;

        gl_Position = shadowData.matrices[layerIndex] * gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = shadowData.matrices[layerIndex] * gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = shadowData.matrices[layerIndex] * gl_in[2].gl_Position;
        EmitVertex();

        EndPrimitive();
    }
}
