#version 330

#define NEAR_PLANE 0
#define FAR_PLANE 1
#define MIN_RADIUS_THRESHOLD 2
#define M_PI 3.14159265f

uniform sampler2D depthTex;
uniform sampler2D normalAndAmbientTex;
uniform sampler2D noiseTex;

in vec2 textCoordinates;
uniform vec3 samples[#KERNEL_SAMPLES#];
uniform mat4 mInverseViewProjection;
uniform mat4 mProjection;
uniform float cameraPlanes[2];
uniform vec2 invResolution;
uniform vec2 resolution;
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

/*
 * Return 0.0 in case of no AO, otherwise a positive number
 */
float computeAO(vec3 position, vec3 normal, vec3 inspectPosition){
	vec3 V = inspectPosition - position;

	float Vlength = length(V);
	float normalDotV = dot(normal, V/Vlength);

    return max(normalDotV - #BIAS_ANGLE#, 0.0f);
}

float linearizeDepth(float depthValue){
	float unmapDepthValue = depthValue * 2.0f - 1.0f;
	return (2.0f * cameraPlanes[NEAR_PLANE]) / (cameraPlanes[FAR_PLANE] + cameraPlanes[NEAR_PLANE] - 
			unmapDepthValue * (cameraPlanes[FAR_PLANE] - cameraPlanes[NEAR_PLANE])); //[0.0=nearPlane, 1.0=far plane]
}

float toWorldDepthValue(float linearizedDepthValue){
	return linearizedDepthValue * (cameraPlanes[FAR_PLANE] - cameraPlanes[NEAR_PLANE]) + cameraPlanes[NEAR_PLANE];
}

void main(){
    vec2 noiseScale = vec2(resolution.x / #NOISE_TEXTURE_SIZE#, resolution.y / #NOISE_TEXTURE_SIZE#);

    float depthValue = texture2D(depthTex, textCoordinates).r;
    vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = texture2D(normalAndAmbientTex, textCoordinates).xyz * 2.0f - 1.0f;

	//TODO vec3 randomVector = normalize(texture(noiseTex, textCoordinates * noiseScale).xyz * 2.0f - 1.0f);
	vec3 randomVector = normalize(vec3(1.0, 1.0, 0.0));

    normal = normalize(normal); //TODO useful ?
	vec3 tangent = normalize(randomVector - dot(randomVector, normal) * normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 kernelMatrix = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < #KERNEL_SAMPLES#; ++i)
    {
        vec3 sampleVectorView = kernelMatrix * samples[i]; //from tangent to view-space
        vec4 samplePointView = vec4(position, 1.0) + #RADIUS# * vec4(sampleVectorView, 0.0);

        vec4 samplePointNDC = mProjection * samplePointView;
        samplePointNDC /= samplePointNDC.w;
        vec2 samplePointTexCoord = samplePointNDC.xy * 0.5 + 0.5;
        float zSceneNDC = texture(depthTex, samplePointTexCoord).r * 2.0 - 1.0;
        float delta = samplePointNDC.z - zSceneNDC;

        // If scene fragment is before (smaller in z) sample point, increase occlusion.
        if (delta > 0.0001 && delta < 0.005)
        {
            occlusion += 1.0;
        }
    }

    occlusion = 1.0 - occlusion / (float(#KERNEL_SAMPLES#) - 1.0);
    fragColor = occlusion;

	//DEBUG: display random texture
/*	fragColor = texture(noiseTex, textCoordinates * noiseScale).x; */






    //TODO fix visual bug on house wall
    //TODO 'stepSizePixel' should be different in function of perspective
    //TODO see https://www.gamasutra.com/view/news/179308/Indepth_Anglebased_SSAO.php : Dealing with large depth differences
/*	float depthValue = texture2D(depthTex, textCoordinates).r;
	float linearizedDepthValue = linearizeDepth(depthValue);
	float zScreenRadius = nearPlaneScreenRadius / toWorldDepthValue(linearizedDepthValue); //radius in pixel at position.z
	if(zScreenRadius < MIN_RADIUS_THRESHOLD){
		fragColor = 0.0f;
		return;
	}
	
	vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = texture2D(normalAndAmbientTex, textCoordinates).xyz  * 2.0f - 1.0f;
	vec3 randomValues = texture2D(randomTex, (textCoordinates/(invResolution*#RANDOM_TEXTURE_SIZE#)) * #TEXTURE_SIZE_FACTOR#).xyz;

	float rotationAngleStep = 2.0f*M_PI / #NUM_DIRECTIONS#;
	float distancePixelStep = (zScreenRadius / float(#NUM_STEPS#)) * randomValues.z;
    float proratedDivisor = (#NUM_STEPS# * (#NUM_STEPS# + 1)) / 2.0f; //=1+2+3+...+#NUM_STEPS#

	float AO = 0.0f;
	float rotationAngle = 0.0f;
	for(int directionIndex=0; directionIndex<#NUM_DIRECTIONS#; ++directionIndex){
		vec2 rayDirection = vec2(sin(rotationAngle), cos(rotationAngle));
		rayDirection = rotateDirection(rayDirection, randomValues.xy);

		for(int distanceIndex=0; distanceIndex<#NUM_STEPS#; ++distanceIndex){
		    float rayDistance = distancePixelStep * (distanceIndex + 1.0f);

			vec2 uvShift = (rayDirection * rayDistance) * invResolution;
			vec3 inspectPosition = fetchPosition(textCoordinates + uvShift);

			float rawAO = computeAO(position, normal, inspectPosition);
			float distanceProratedAO = rawAO * ((#NUM_STEPS# - distanceIndex) / proratedDivisor);
			AO += distanceProratedAO;
		}
		rotationAngle += rotationAngleStep;
	}
	
	AO = clamp(AO / float(#NUM_DIRECTIONS#), 0.0f, 1.0f);
	fragColor = pow(AO, #AO_EXPONENT#); */

	//DEBUG: display scope radius at screen center point
/*	float centerDepthValue = texture2D(depthTex, vec2(0.5, 0.5)).r;
	float centerLinearizedDepthValue = linearizeDepth(centerDepthValue);
	float centerZScreenRadius = nearPlaneScreenRadius / toWorldDepthValue(centerLinearizedDepthValue);
	float lengthToCenter = length((textCoordinates - vec2(0.5, 0.5)) / invResolution);
	if(lengthToCenter < centerZScreenRadius){
		fragColor = 1.0f;
	}*/

	//DEBUG: display random texture
/*	fragColor = randomValues.b;*/
}
