#version 440

//values are replaced at compilation time:
#define MAX_VERTICES 0
#define NUMBER_LAYER 0

layout(triangles) in;
layout(triangle_strip, max_vertices=MAX_VERTICES) out;

uniform unsigned int layersToUpdate;

const unsigned int POWER_TWO_TAB[13] = unsigned int[](1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096);

in vec2 textCoordinates[];

out int gl_Layer;
smooth out vec2 vertexTextCoordinates;

void main(){
	for(int layer=0; layer<NUMBER_LAYER; layer++){
		if((layersToUpdate & POWER_TWO_TAB[layer]) != uint(0)){
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
