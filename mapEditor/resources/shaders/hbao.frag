#version 330

#define NEAR_PLANE 0
#define FAR_PLANE 1
#define M_PI 3.14159265f

uniform sampler2D depthTex;
uniform sampler2D normalAndAmbientTex;

in vec2 textCoordinates;
uniform mat4 mInverseViewProjection;
uniform float cameraPlanes[2];
uniform vec2 invResolution;
uniform float nearPlaneScreenRadius;

layout (location = 0) out float fragColor;

vec3 fetchPosition(vec2 textCoord, float depthValue){
	vec4 texPosition = vec4(
		textCoord.s * 2.0f - 1.0f,
		textCoord.t * 2.0f - 1.0f,
		depthValue,
		1.0
	);
	vec4 position = mInverseViewProjection * texPosition;
	position /= position.w;
	return vec3(position);
}

vec3 fetchPosition(vec2 textCoord){
	float depthValue = texture2D(depthTex, textCoord).r * 2.0f - 1.0f;
	return fetchPosition(textCoord, depthValue);
}

float computeAO(vec3 position, vec3 normal, vec3 inspectPosition){
	vec3 V = inspectPosition - position;

	float Vlength = length(V);
	float normalDotV = dot(normal, V/Vlength);
	
  	float linearAttenuationFactor = clamp(1.0f - (Vlength / #RADIUS#), 0.0f, 1.0f);
	return max(normalDotV, 0) * linearAttenuationFactor;
}

void main(){
	float depthValue = texture2D(depthTex, textCoordinates).r * 2.0f - 1.0f;
	if(depthValue >= 0.99999f){ //nothing display
		fragColor = 0.0f;
		return;
	}
	
	float linearizedDepthValue = (2.0f * cameraPlanes[NEAR_PLANE]) / (cameraPlanes[FAR_PLANE] + cameraPlanes[NEAR_PLANE] - 
			depthValue * (cameraPlanes[FAR_PLANE] - cameraPlanes[NEAR_PLANE])); //[0.0=nearPlane, 1.0=far plane]	
	vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = texture2D(normalAndAmbientTex, textCoordinates).xyz  * 2.0f - 1.0f;
	
	float rotationAngleStep = 2.0f*M_PI / #NUM_DIRECTIONS#;
	float zScreenRadius = nearPlaneScreenRadius / linearizedDepthValue; //radius in pixel at position.z plane
	float stepSizePixel = zScreenRadius / (#NUM_STEPS# +1); //+1: avoid total attenuation at last step
	float AO = 0.0f;
	
	float rotationAngle = 0.0f;
	for(int directionIndex=0; directionIndex<#NUM_DIRECTIONS#; ++directionIndex){
		vec2 direction = vec2(sin(rotationAngle), cos(rotationAngle)); //TODO limit direction to semi-spherical around the normal... (see http://john-chapman-graphics.blogspot.be/2013/01/ssao-tutorial.html)
		
		float raySizePixel = stepSizePixel;
		for(int stepIndex=0; stepIndex<#NUM_STEPS#; ++stepIndex){
			vec2 uvShift = (direction * raySizePixel) * invResolution;
			vec3 inspectPosition = fetchPosition(textCoordinates + uvShift);
			
			AO += computeAO(position, normal, inspectPosition);
			
			raySizePixel += stepSizePixel;
		}
		rotationAngle += rotationAngleStep;
	}
	
	AO = AO / (#NUM_DIRECTIONS# * #NUM_STEPS#);
	fragColor = clamp(1.0 - AO * 2.0, 0, 1);
}
