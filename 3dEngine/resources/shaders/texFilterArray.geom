#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 texCoordinates[];

layout(location = 0) out vec3 vertexTexCoordinates;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main() {
    gl_Layer = int(texCoordinates[0].z);

    gl_Position = gl_in[0].gl_Position;
    vertexTexCoordinates = texCoordinates[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    vertexTexCoordinates = texCoordinates[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    vertexTexCoordinates = texCoordinates[2];
    EmitVertex();

    EndPrimitive();
}
