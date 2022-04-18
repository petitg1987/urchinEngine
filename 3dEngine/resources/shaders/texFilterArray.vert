#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

//instance data
layout(location = 2) in float texLayerId;

layout(location = 0) out vec3 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = vec3(texCoord, texLayerId);
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}
