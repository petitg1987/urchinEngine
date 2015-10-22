#version 330

layout (location = 0) out vec2 fragColor;

void main(){
	float depth = gl_FragCoord.z / gl_FragCoord.w;
	fragColor.x = depth;
	//fragColor.y = depth * depth;
	
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	fragColor.y = pow(depth, 2.0) + 0.25*(dx*dx + dy*dy);
}