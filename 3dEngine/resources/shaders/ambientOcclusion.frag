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
uniform mat4 mView;
uniform vec2 resolution;

layout (location = 0) out float fragColor;

vec3 fetchEyePosition(vec2 textCoord, float depthValue){
	vec4 texPosition = vec4(
		textCoord.s * 2.0f - 1.0f,
		textCoord.t * 2.0f - 1.0f,
		depthValue * 2.0f - 1.0f,
		1.0
	);
	vec4 position = inverse(mProjection) * texPosition;
	position /= position.w;
	return vec3(position);
}

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
float computeAO(vec3 position, vec3 normal, vec3 inspectPosition){ //TODO remove or use it
	vec3 V = inspectPosition - position;

	float Vlength = length(V);
	float normalDotV = dot(normal, V/Vlength);

    return max(normalDotV - #BIAS_ANGLE#, 0.0f);
}

void main(){
    vec2 noiseScale = vec2(resolution.x / #NOISE_TEXTURE_SIZE#, resolution.y / #NOISE_TEXTURE_SIZE#);

    float depthValue = texture2D(depthTex, textCoordinates).r;
    vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = texture2D(normalAndAmbientTex, textCoordinates).xyz * 2.0f - 1.0f;

	vec3 randomVector = normalize(texture(noiseTex, textCoordinates * noiseScale).xyz * 2.0f - 1.0f);

	vec3 tangent = normalize(randomVector - dot(randomVector, normal) * normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 kernelMatrix = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < #KERNEL_SAMPLES#; ++i)
    {
        vec3 sampleVectorWorldSpace = kernelMatrix * samples[i];
        vec3 samplePointWorldSpace = position + #RADIUS# * sampleVectorWorldSpace;
        vec4 samplePointEyeSpace = mView * vec4(samplePointWorldSpace, 1.0);
        vec4 samplePointClipSpace = mProjection * samplePointEyeSpace;
        vec3 samplePointNDC = samplePointClipSpace.xyz / samplePointClipSpace.w;
        vec2 samplePointTexCoord = samplePointNDC.xy * 0.5 + 0.5;

        float zSceneNDC = texture(depthTex, samplePointTexCoord).r;
        vec3 scenePositionEyeSpace = fetchEyePosition(samplePointTexCoord, zSceneNDC);

        float rangeCheck = smoothstep(0.0, 1.0, #RADIUS# / abs(scenePositionEyeSpace.z - samplePointEyeSpace.z));
        occlusion += (scenePositionEyeSpace.z >= samplePointEyeSpace.z + 0.15 ? 1.0 : 0.0) * rangeCheck; //TODO configure bias or use BIAS_ANGLE ?
    }

    fragColor = (occlusion / float(#KERNEL_SAMPLES#));

	//DEBUG: display random texture
/*	fragColor = texture(noiseTex, textCoordinates * noiseScale).x; */
}
