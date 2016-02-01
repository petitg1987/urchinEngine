#version 330

#define NEAR_PLANE 0
#define FAR_PLANE 1

uniform sampler2D tex;
uniform sampler2D depthTex;
uniform float cameraPlanes[2];

in vec2 textCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

float linearizeDepth(float depthValue){
	return (2.0f * cameraPlanes[NEAR_PLANE]) / (cameraPlanes[FAR_PLANE] + cameraPlanes[NEAR_PLANE] - 
			depthValue * (cameraPlanes[FAR_PLANE] - cameraPlanes[NEAR_PLANE])); //[0.0=nearPlane, 1.0=far plane]
}

float bilateralBlur(vec2 uvOffset, int r, float linearizedDepthCenterValue, inout float totalWeight){
	float texValue = texture2D(tex, textCoordinates+uvOffset).r;
	float depthValue = texture2D(depthTex, textCoordinates).r;
	float linearizedDepthValue = linearizeDepth(depthValue);
	
	const float blurSigma = float(#KERNEL_RADIUS#) * 0.5f;
  	const float blurFalloff = 1.0 / (2.0*blurSigma*blurSigma);
	float ddiff = (linearizedDepthValue - linearizedDepthCenterValue) * #BLUR_SHARPNESS#;
	float weight = exp2(-r*r*blurFalloff - ddiff*ddiff);

	totalWeight += weight;
	
	return texValue * weight;
}

void main(){
	float centerTexValue = texture2D(tex, textCoordinates).r;
	float depthCenterValue = texture2D(depthTex, textCoordinates).r;
	float linearizedDepthCenterValue = linearizeDepth(depthCenterValue);

	float offsets[] = float[](#OFFSETS_TAB#);
	
	fragColor = centerTexValue;
	float totalWeight = 1.0f;
	
	#LOOP1_START(#KERNEL_RADIUS#)#		
		vec2 uvOffset = (#IS_VERTICAL_BLUR#) ? vec2(0.0, offsets[#LOOP1_COUNTER#]) : vec2(offsets[#LOOP1_COUNTER#], 0.0);
		fragColor += bilateralBlur(uvOffset, #LOOP1_COUNTER#+1, linearizedDepthCenterValue, totalWeight);
	#LOOP1_END#
	
	#LOOP2_START(#KERNEL_RADIUS#)#		
		vec2 uvOffset = (#IS_VERTICAL_BLUR#) ? vec2(0.0, -offsets[#LOOP2_COUNTER#]) : vec2(-offsets[#LOOP2_COUNTER#], 0.0);
		fragColor += bilateralBlur(uvOffset, #LOOP2_COUNTER#+1, linearizedDepthCenterValue, totalWeight);
	#LOOP2_END#
	
	fragColor /= totalWeight;
}