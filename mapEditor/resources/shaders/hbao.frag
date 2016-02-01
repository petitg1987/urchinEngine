#version 330

#define NEAR_PLANE 0
#define FAR_PLANE 1
#define MIN_RADIUS_THRESHOLD 2
#define M_PI 3.14159265f

uniform sampler2D depthTex;
uniform sampler2D normalAndAmbientTex;
uniform sampler2D randomTex;

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
		depthValue * 2.0f - 1.0f,
		1.0
	);
	vec4 position = mInverseViewProjection * texPosition;
	position /= position.w;
	return vec3(position);
}

vec3 fetchPosition(vec2 textCoord){
	float depthValue = texture2D(depthTex, textCoord).r;
	return fetchPosition(textCoord, depthValue);
}

vec2 rotateDirection(vec2 direction, vec2 sinCos)
{
	return vec2(	direction.x*sinCos.x - direction.y*sinCos.y, 
  					direction.x*sinCos.y + direction.y*sinCos.x);
}

float computeAO(vec3 position, vec3 normal, vec3 inspectPosition){
	vec3 V = inspectPosition - position;

	float Vlength = length(V);
	float normalDotV = dot(normal, V/Vlength);
  	
  	float linearAttenuationFactor = clamp(1.0f - (Vlength / #RADIUS#), 0.0f, 1.0f);
	return max(normalDotV - #BIAS_ANGLE#, 0.0f) * linearAttenuationFactor;
}

float linearizeDepth(float depthValue){
	return (2.0f * cameraPlanes[NEAR_PLANE]) / (cameraPlanes[FAR_PLANE] + cameraPlanes[NEAR_PLANE] - 
			depthValue * (cameraPlanes[FAR_PLANE] - cameraPlanes[NEAR_PLANE])); //[0.0=nearPlane, 1.0=far plane]
}

void main(){
	float depthValue = texture2D(depthTex, textCoordinates).r;	
	float linearizedDepthValue = linearizeDepth(depthValue);
	float zScreenRadius = nearPlaneScreenRadius / linearizedDepthValue; //radius in pixel at position.z
	if(zScreenRadius < MIN_RADIUS_THRESHOLD){
		fragColor = 0.0f;
		return;
	}
	
	vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = texture2D(normalAndAmbientTex, textCoordinates).xyz  * 2.0f - 1.0f;
	vec3 randomValues = texture2D(randomTex, (textCoordinates/(invResolution*#RANDOM_TEXTURE_SIZE#)) * #TEXTURE_SIZE_FACTOR#).xyz;
		
	int numStepsAdjusted = min(#NUM_STEPS#, int(zScreenRadius - 1)); //avoid too much steps in case of small radius
	float stepSizePixel = zScreenRadius / (float(numStepsAdjusted) + 1.0f); //+1: avoid total attenuation at last step
	float halfStepSizePixel = stepSizePixel / 2.0f;
	
	int numDirectionsAdjusted = min(#NUM_DIRECTIONS#, int(zScreenRadius - 1)); //avoid too much directions in case of small radius
	float rotationAngleStep = 2.0f*M_PI / numDirectionsAdjusted;

	float AO = 0.0f;
	float rotationAngle = 0.0f;
	for(int directionIndex=0; directionIndex<numDirectionsAdjusted; ++directionIndex){
		vec2 direction = vec2(sin(rotationAngle), cos(rotationAngle));
		vec2 randomizedDirection = rotateDirection(direction, randomValues.xy);
		
		float raySizePixel = stepSizePixel;
		float randomizedRaySizePixel = (raySizePixel * randomValues.z) + halfStepSizePixel;
		for(int stepIndex=0; stepIndex<numStepsAdjusted; ++stepIndex){
			vec2 uvShift = (randomizedDirection * randomizedRaySizePixel) * invResolution;
			vec3 inspectPosition = fetchPosition(textCoordinates + uvShift);
			
			AO += computeAO(position, normal, inspectPosition);
			
			randomizedRaySizePixel += stepSizePixel;
		}
		rotationAngle += rotationAngleStep;
	}
	
	AO = clamp(AO / (numDirectionsAdjusted * numStepsAdjusted), 0.0f, 1.0f);
	fragColor = pow(AO, #AO_EXPONENT#);

	//DEBUG: display scope radius at screen center point
/*	float centerDepthValue = texture2D(depthTex, vec2(0.5, 0.5)).r * 2.0f - 1.0f;
	float centerLinearizedDepthValue = linearizeDepth(centerDepthValue);
	float centerZScreenRadius = nearPlaneScreenRadius / centerLinearizedDepthValue;
	float lengthToCenter = length((textCoordinates - vec2(0.5, 0.5)) / invResolution);
	if(lengthToCenter < centerZScreenRadius){
		fragColor = 1.0f;
	}*/
	
	//DEBUG: display random texture
/*	fragColor = randomValues.b;*/
}
