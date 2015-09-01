#version 330

layout(location=0) in vec2 vertexPosition;
layout(location=1) in vec2 texCoord;

uniform mat3 mProjection;

out vec2 textCoordinates;
invariant gl_Position;

void main(){
	textCoordinates = texCoord;
	
	vec3 position = mProjection * vec3(vertexPosition, 1.0);
	position /= position.z;
	
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}