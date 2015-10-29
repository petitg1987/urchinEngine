#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices=#MAX_VERTICES#) out;

uniform mat4 projectionMatrix[#NUMBER_SHADOW_MAPS#];

uniform int firstSplit;
uniform int lastSplit;

out int gl_Layer;

void main(){
	for(int layer=firstSplit; layer<=lastSplit; layer++){
		gl_Layer = layer;

		gl_Position = projectionMatrix[layer] * gl_in[0].gl_Position;
		EmitVertex();
		gl_Position = projectionMatrix[layer] * gl_in[1].gl_Position;
		EmitVertex();
		gl_Position = projectionMatrix[layer] * gl_in[2].gl_Position;
		EmitVertex();

		EndPrimitive();
	}
}
