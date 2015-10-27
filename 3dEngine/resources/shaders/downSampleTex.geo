#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices=#TOKEN0#) out;

in vec2 textCoordinates[];

out int gl_Layer;
smooth out vec2 vertexTextCoordinates;

void main(){
	#LOOP1_START(#TOKEN1#)#
		gl_Layer = #LOOP1_COUNTER#;

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
	#LOOP1_END#
}
