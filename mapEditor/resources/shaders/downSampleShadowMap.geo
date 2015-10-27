#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices=#TOKEN0#) out;

out int gl_Layer;

void main(){
	#LOOP1_START(#TOKEN1#)#
		gl_Layer = #TOKEN1#;

		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();

		EndPrimitive();
	#LOOP1_END#
}
