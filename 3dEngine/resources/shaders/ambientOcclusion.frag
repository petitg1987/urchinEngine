#version 440

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

void main(){
    vec4 normalAndAmbient = vec4(texture2D(normalAndAmbientTex, textCoordinates));
    if(normalAndAmbient.a >= 0.99999f){ //no lighting
    	fragColor = 0.0;
    	return;
    }

    float depthValue = texture2D(depthTex, textCoordinates).r;
    float distanceReduceFactor = 1.0;
    if(depthValue > #DEPTH_END_ATTENUATION#){
        fragColor = 0.0;
        return;
    }else if(depthValue > #DEPTH_START_ATTENUATION#){
        distanceReduceFactor = (#DEPTH_END_ATTENUATION# - depthValue) / (#DEPTH_END_ATTENUATION# - #DEPTH_START_ATTENUATION#);
    }

    vec3 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = normalAndAmbient.xyz * 2.0f - 1.0f;
	vec2 noiseScale = vec2(resolution.x / #NOISE_TEXTURE_SIZE#, resolution.y / #NOISE_TEXTURE_SIZE#);
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
        occlusion += (scenePositionEyeSpace.z >= samplePointEyeSpace.z + #BIAS# ? 1.0 : 0.0) * rangeCheck;
    }

    fragColor = (occlusion / float(#KERNEL_SAMPLES#)) * distanceReduceFactor * #AO_STRENGTH#;

	//DEBUG: display random texture
/*	fragColor = texture(noiseTex, textCoordinates * noiseScale).x; */
}
