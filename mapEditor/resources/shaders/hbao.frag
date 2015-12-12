#version 330

uniform sampler2D depthTex;

in vec2 textCoordinates;
uniform mat4 mInverseViewProjection;

layout (location = 0) out float fragColor;

void main(){		
	vec4 texPosition = vec4(
		textCoordinates.s * 2.0f - 1.0f,
		textCoordinates.t * 2.0f - 1.0f,
		texture2D(depthTex, textCoordinates).r * 2.0f - 1.0f,
		1.0
	);
	vec4 position = mInverseViewProjection * texPosition;
	position /= position.w;
	
	fragColor = 1.0f;
}