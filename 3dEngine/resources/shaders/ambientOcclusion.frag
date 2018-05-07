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

	vec3 randomVector = normalize(texture(noiseTex, textCoordinates * noiseScale).xyz * 2.0f - 1.0f);

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
        samplePointNDC.xyz /= samplePointNDC.w;

        vec2 samplePointTexCoord = samplePointNDC.xy * 0.5 + 0.5;

        float zSceneNDC = texture(depthTex, samplePointTexCoord).r;
        vec3 scenePosition = fetchPosition(samplePointTexCoord, zSceneNDC); //TODO alternative: linearize depth
        occlusion += (scenePosition.z >= samplePointView.z + 0.015 ? 1.0 : 0.0);
    }

    occlusion = 1.0 - (occlusion / float(#KERNEL_SAMPLES#));
    fragColor = occlusion;

	//DEBUG: display random texture
/*	fragColor = texture(noiseTex, textCoordinates * noiseScale).x; */
}
