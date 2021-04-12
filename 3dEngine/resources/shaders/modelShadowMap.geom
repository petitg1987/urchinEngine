#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define MAX_VERTICES 3 * 15 //must be equals to LightManager::LIGHTS_LIMIT //TODO update comment in LightManager::LIGHTS_LIMIT & use shader const
#define NUMBER_SHADOW_MAPS 0

layout(std140, set = 0, binding = 2) uniform Projection {
    mat4 matrices[NUMBER_SHADOW_MAPS];
} projection;
layout(std140, set = 0, binding = 3) uniform Layer {
    int layersToUpdate;
} layer;

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

const uint POWER_TWO_TAB[13] = uint[](1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096);

void main() {
    for (int layerIndex = 0; layerIndex < NUMBER_SHADOW_MAPS; layerIndex++) {
        if ((uint(layer.layersToUpdate) & POWER_TWO_TAB[layerIndex]) != uint(0)) {
            gl_Layer = layerIndex;

            gl_Position = projection.matrices[layerIndex] * gl_in[0].gl_Position;
            EmitVertex();
            gl_Position = projection.matrices[layerIndex] * gl_in[1].gl_Position;
            EmitVertex();
            gl_Position = projection.matrices[layerIndex] * gl_in[2].gl_Position;
            EmitVertex();

            EndPrimitive();
        }
    }
}
