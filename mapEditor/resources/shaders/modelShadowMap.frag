#version 330

layout (location = 0) out vec2 fragColor;

void main()
{
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	fragColor.x = depth;
	fragColor.y = depth * depth;
}