#version 330

smooth in vec2 vertexTextCoordinates;
in vec3 grassNormal;

uniform sampler2D grassTex;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
    float fAlphaTest = 0.75f; //TODO configurable ?

    vec4 color = texture2D(grassTex, vertexTextCoordinates);

	if(color.a < fAlphaTest)
		discard;

	fragColor = vec4(color.xyz, color.a);
	fragNormalAndAmbient = vec4(grassNormal, 0.3f); //TODO use ambient from terrain
}
