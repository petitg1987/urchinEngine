#version 330
#extension GL_EXT_gpu_shader4 : enable

//deferred textures:
//|-------------------------------------------------------------------|
//| Depth (32 bits)                                                   |
//|-------------------------------------------------------------------|
//| Diffuse red    | Diffuse green  | Diffuse blue   | EMPTY          |
//|-------------------------------------------------------------------|
//| Normal X       | Normal Y       | Normal Z       | Ambient factor |
//|-------------------------------------------------------------------|
//| Ambient occlusion factor (16 bits)                                |
//|-------------------------------------------------------------------|
in vec2 textCoordinates;
uniform sampler2D depthTex;
uniform sampler2D colorTex;
uniform sampler2D normalAndAmbientTex;
uniform sampler2D ambientOcclusionTex;
uniform mat4 mInverseViewProjection;
uniform bool hasShadow;
uniform bool hasAmbientOcclusion;

//lights and shadows:
struct StructLightInfo{
	bool isExist;
	bool produceShadow;
	bool hasParallelBeams;
	vec3 positionOrDirection;
	
	float exponentialAttenuation;
	vec3 lightAmbient;
	
	sampler2DArray shadowMapTex;

	mat4 mLightProjectionView[#NUMBER_SHADOW_MAPS#];
};
uniform StructLightInfo lightsInfo[#MAX_LIGHTS#];
uniform float depthSplitDistance[#NUMBER_SHADOW_MAPS#];
uniform vec4 globalAmbient;

layout (location = #OUTPUT_LOCATION#) out vec4 fragColor;

vec4 fetchPosition(vec2 textCoord, float depthValue){
	vec4 texPosition = vec4(
		textCoord.s * 2.0f - 1.0f,
		textCoord.t * 2.0f - 1.0f,
		depthValue * 2.0f - 1.0f,
		1.0
	);
	vec4 position = mInverseViewProjection * texPosition;
	position /= position.w;
	return position;
}

float linearStep(float min, float max, float v){
  	return clamp((v - min) / (max - min), 0.0f, 1.0f);  
} 

float computePercentLit(float shadowMapZ, vec2 moments){
    float isInHardShadow = float(shadowMapZ <= moments.x);
    
    float variance = moments.y - (moments.x*moments.x);
    variance = max(variance, #SHADOW_MAP_BIAS#); //reduce fake shadow between splitted shadow maps
    
    float d = moments.x - shadowMapZ;
    float pMax = variance / (variance + d*d);
    
    pMax = linearStep(0.4f, 1.0f, pMax); //reduce light bleeding
    
    return max(isInHardShadow, pMax);
}

float computeShadowContribution(int lightIndex, float depthValue, vec4 position){
	float shadowContribution = 1.0;
	
	if(lightsInfo[lightIndex].produceShadow){
		#LOOP1_START(#NUMBER_SHADOW_MAPS#)#
			if(depthValue < depthSplitDistance[#LOOP1_COUNTER#]){
			
				vec4 shadowCoord = (((lightsInfo[lightIndex].mLightProjectionView[#LOOP1_COUNTER#] * position) / 2.0) + 0.5);

				//model has produceShadow flag to true ?
				if(shadowCoord.s<=1.0 && shadowCoord.s>=0.0 && shadowCoord.t<=1.0 && shadowCoord.t>=0.0){
					vec2 moments = texture2DArray(lightsInfo[lightIndex].shadowMapTex, vec3(shadowCoord.st, #LOOP1_COUNTER#)).rg;

					shadowContribution = computePercentLit(shadowCoord.z, moments);
					
					#LOOP1_IF_LAST_ITERATION#
						//shadow attenuation before disappear
						float startAttenuation = depthSplitDistance[#NUMBER_SHADOW_MAPS#-1] * 0.9996;
						shadowContribution = min(1.0f, shadowContribution 
								+ max(0.0f, ((depthValue - startAttenuation)/(depthSplitDistance[#NUMBER_SHADOW_MAPS#-1] - startAttenuation))));
					#LOOP1_ENDIF_LAST_ITERATION#
				}

				#LOOP1_STOP#
			}
		#LOOP1_END#
	}
	
	return shadowContribution;
}

void main(){	
	vec4 diffuse = texture2D(colorTex, textCoordinates);
	vec4 normalAndAmbient = vec4(texture2D(normalAndAmbientTex, textCoordinates));
	float modelAmbientFactor = normalAndAmbient.a;
	
	if(modelAmbientFactor >= 0.99999f){ //no lighting
		fragColor = diffuse;
		return;
	}
	
	float depthValue = texture2D(depthTex, textCoordinates).r;
	vec4 position = fetchPosition(textCoordinates, depthValue);
	vec3 normal = vec3(normalAndAmbient) * 2.0f - 1.0f;
	vec4 modelAmbient = diffuse * modelAmbientFactor;
	
	fragColor = globalAmbient * modelAmbient;
	
	if(hasAmbientOcclusion){
		float ambienOcclusionFactor = texture2D(ambientOcclusionTex, textCoordinates).r;
		fragColor *= (1.0f - ambienOcclusionFactor);
	}
		
	for(int i=0; i<#MAX_LIGHTS#;++i){
		if(lightsInfo[i].isExist){
			vec3 vertexToLightNormalized;
			float lightAttenuation;
				
			if(lightsInfo[i].hasParallelBeams){ //sun light
				vec3 vertexToLight = -lightsInfo[i].positionOrDirection;
				vertexToLightNormalized = normalize(vertexToLight);
				lightAttenuation = 1.0f;
			}else{ //omnidirectional light
				vec3 vertexToLight = lightsInfo[i].positionOrDirection - vec3(position);
				float dist = length(vertexToLight);
				vertexToLightNormalized = normalize(vertexToLight);
				lightAttenuation = exp(-dist * lightsInfo[i].exponentialAttenuation);
			}
			
			float NdotL = max(dot(normal, vertexToLightNormalized), 0.0f);
			vec4 ambient = vec4(lightsInfo[i].lightAmbient, 0.0f) * modelAmbient;
			
			float percentLit = 1.0;
			if(hasShadow){
				percentLit = computeShadowContribution(i, depthValue, position);
			}
			
			fragColor += lightAttenuation * (percentLit * (diffuse * NdotL) + ambient);
		}else{
			break; //no more light
		}
	}
	
	//DEBUG: add color to shadow map splits
/*	const float colorValue = 0.25f;
	vec4 splitColors[5] = vec4[](
		vec4(colorValue, 0.0, 0.0, 1.0), vec4(0.0, colorValue, 0.0, 1.0), vec4(0.0, 0.0, colorValue, 1.0),
		vec4(colorValue, 0.0, colorValue, 1.0),	vec4(colorValue, colorValue, 0.0, 1.0));
	#LOOP2_START(#NUMBER_SHADOW_MAPS#)#
		if(depthValue < depthSplitDistance[#LOOP2_COUNTER#]){
			fragColor += splitColors[#LOOP2_COUNTER#%5];
			
			#LOOP2_STOP#
		}
	#LOOP2_END# */
}
