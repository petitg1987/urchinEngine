#version 330

#define M_PI 3.14159265f
#define BIAS 0.1f

uniform sampler2D depthTex;
uniform sampler2D normalAndAmbientTex;

in vec2 textCoordinates;
uniform mat4 mInverseViewProjection;
uniform vec2 invResolution;

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

float falloff(float distanceSquare, float radius){
  float meters2viewspace = 1.0f;
  float R = radius * meters2viewspace;
  float negInvR2 = -1.0f / (R * R);
  
  return distanceSquare * negInvR2 + 1.0f;
}

float computeAO(vec3 position, vec3 normal, vec3 inspectPosition, float radius){
	vec3 V = inspectPosition - position;
	float VdotV = dot(V, V);
	float NdotV = dot(normal, V) * 1.0/sqrt(VdotV);

	const float bias = BIAS;
	const float NDotVBias = min(max(0.0f, bias),1.0f);
  
	return clamp(NdotV - NDotVBias, 0, 1) * clamp(falloff(VdotV, radius), 0, 1);
}

void main(){
	float depthValue = texture2D(depthTex, textCoordinates).r * 2.0f - 1.0f;
	if(depthValue >= 0.99999f){ //nothing display
		fragColor = 0.0f;
		return;
	}
	
	vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = texture2D(normalAndAmbientTex, textCoordinates).xyz  * 2.0f - 1.0f;
	
	float rotationAngleStep = 2.0f*M_PI / #NUM_DIRECTIONS#.0f;
	float radius = #RADIUS#;
	float stepSizePixel = radius / #NUM_STEPS#;
	float AO = 0.0f;
	
	float rotationAngle = 0.0f;
	for(int directionIndex=0; directionIndex<#NUM_DIRECTIONS#; ++directionIndex){
		vec2 direction = vec2(sin(rotationAngle), cos(rotationAngle)); //TODO limit direction to semi-spherical around the normal... (see http://john-chapman-graphics.blogspot.be/2013/01/ssao-tutorial.html)
		
		float raySizePixel = stepSizePixel;
		for(int stepIndex=0; stepIndex<#NUM_STEPS#; ++stepIndex){
			vec2 uvShift = (direction * raySizePixel) * invResolution;
			vec3 inspectPosition = fetchPosition(textCoordinates + uvShift);
			
			AO += computeAO(position, normal, inspectPosition, radius);
			
			raySizePixel += stepSizePixel;
		}
		rotationAngle += rotationAngleStep;
	}
	
	const float bias = BIAS;
  	const float NDotVBias = min(max(0.0f, bias),1.0f);
  	const float AOMultiplier = 1.0f / (1.0f - NDotVBias);
	AO *= AOMultiplier / (#NUM_DIRECTIONS# * #NUM_STEPS#);
	
	float AOClamped = clamp(1.0 - AO * 2.0, 0, 1);
	
	const float intensity = 2.0f;
	const float powExponent = max(intensity, 0.0f);

	fragColor = pow(AOClamped, powExponent);
}
