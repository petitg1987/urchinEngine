#version 330

in vec2 textCoordinates;
in vec3 normal;

uniform sampler2D maskTex;
uniform sampler2D diffuseTex1;
uniform sampler2D diffuseTex2;
uniform sampler2D diffuseTex3;
uniform sampler2D diffuseTex4;
uniform float ambient;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
	//diffuse
	vec2 maskTextCoordinates2 = vec2(textCoordinates.x/15.0, textCoordinates.y/15.0); //TODO hardcoded
	vec4 maskValue = texture2D(maskTex, maskTextCoordinates2);

	vec4 diffuseValue1 = texture2D(diffuseTex1, textCoordinates);
	vec4 diffuseValue2 = texture2D(diffuseTex2, textCoordinates); //TODO avoid read unused texture ?
	vec4 diffuseValue3 = texture2D(diffuseTex3, textCoordinates);
	vec4 diffuseValue4 = texture2D(diffuseTex4, textCoordinates);

	fragColor = maskValue.r * diffuseValue1
	    + maskValue.g * diffuseValue2
	    + maskValue.b * diffuseValue3
	    + maskValue.a * diffuseValue4;

	//material
	vec3 texNormal = (normal + 1.0) / 2.0;
	fragNormalAndAmbient = vec4(texNormal, ambient);
}
