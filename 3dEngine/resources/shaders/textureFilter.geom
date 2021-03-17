#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define MAX_VERTICES 0
#define NUMBER_LAYER 0

uniform int layersToUpdate; //binding 0

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

in vec2 textCoordinates[];

smooth out vec2 vertexTextCoordinates;

const uint POWER_TWO_TAB[13] = uint[](1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096);

void main() {
    for (int layer = 0; layer < NUMBER_LAYER; layer++) {
        if ((uint(layersToUpdate) & POWER_TWO_TAB[layer]) != uint(0)) {
            gl_Layer = layer;

            gl_Position = gl_in[0].gl_Position;
            vertexTextCoordinates = textCoordinates[0];
            EmitVertex();
            gl_Position = gl_in[1].gl_Position;
            vertexTextCoordinates = textCoordinates[1];
            EmitVertex();
            gl_Position = gl_in[2].gl_Position;
            vertexTextCoordinates = textCoordinates[2];
            EmitVertex();

            EndPrimitive();
        }
    }
}
