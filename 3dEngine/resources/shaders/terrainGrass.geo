#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

void main(){

    for(int i = 0; i < 3; i++)
	{
        //top right
        gl_Position = gl_in[0].gl_Position + vec4(0.3, 0.2, 0.0, 0.0);
        EmitVertex();

        //bottom right
        gl_Position = gl_in[0].gl_Position + vec4(0.3, 0.0, 0.0, 0.0);
        EmitVertex();

        //top-left
        gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.2, 0.0, 0.0);
        EmitVertex();

        //bottom-left
        gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
        EmitVertex();

        EndPrimitive();
	}
}
