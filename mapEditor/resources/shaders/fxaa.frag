#version 330

in vec2 textCoordinates;

uniform sampler2D tex;
uniform vec2 invSceneSize;

out vec4 fragColor;

void main()
{
	const float FXAA_EDGE_THRESHOLD = 1.0/3.0;
	const float FXAA_SPAN_MAX = 6.0;
	const float FXAA_REDUCE_MUL = 1.0/8.0;
	const float FXAA_REDUCE_MIN = 1.0/32.0;
	
	vec3 rgbNorthWest = texture2D(tex, textCoordinates.xy + (vec2(-1.0, -1.0) * invSceneSize)).xyz;
	vec3 rgbNorthEst = texture2D(tex, textCoordinates.xy + (vec2(+1.0, -1.0) * invSceneSize)).xyz;
	vec3 rgbSudWest = texture2D(tex, textCoordinates.xy + (vec2(-1.0, +1.0) * invSceneSize)).xyz;
	vec3 rgbSudEst = texture2D(tex, textCoordinates.xy + (vec2(+1.0, +1.0) * invSceneSize)).xyz;
	vec3 rgbMiddle = texture2D(tex, textCoordinates.xy).xyz;
	
	const vec3 lumaFactor = vec3(0.299, 0.587, 0.114);
	float lumaNorthWest = dot(rgbNorthWest, lumaFactor);
	float lumaNorthEst = dot(rgbNorthEst, lumaFactor);
	float lumaSudWest = dot(rgbSudWest, lumaFactor);
	float lumaSudEst = dot(rgbSudEst, lumaFactor);
	float lumaMiddle = dot(rgbMiddle, lumaFactor);
	
	float lumaMin = min(lumaMiddle, min(min(lumaNorthWest, lumaNorthEst), min(lumaSudWest, lumaSudEst)));
	float lumaMax = max(lumaMiddle, max(max(lumaNorthWest, lumaNorthEst), max(lumaSudWest, lumaSudEst)));
	
	if((lumaMax-lumaMin) < lumaMax * FXAA_EDGE_THRESHOLD)
	{
		fragColor = vec4(rgbMiddle, 1.0);
		return;
	}
	
	//compute edge direction
	vec2 edgeDir = vec2(-((lumaNorthWest + lumaNorthEst) - (lumaSudWest + lumaSudEst)),
		((lumaNorthWest + lumaSudWest) - (lumaNorthEst + lumaSudEst)));
	
	//adjust magnitude of direction: make sure that any axe distance is at least equals to 1 pixel distance.
	//FXAA_REDUCE_MIN allow to avoid division by zero.
	edgeDir = edgeDir * (1.0/(min(abs(edgeDir.x), abs(edgeDir.y)) + FXAA_REDUCE_MIN));
	
	//clamp direction to avoid looking on too far pixels
	edgeDir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), edgeDir)) * invSceneSize;
	
	vec3 twoNearSamplesRgb = (1.0/2.0) * (
		texture2D(tex, textCoordinates.xy + edgeDir * (1.0/3.0 - 0.5)).xyz +
		texture2D(tex, textCoordinates.xy + edgeDir * (2.0/3.0 - 0.5)).xyz);
	
	vec3 fourSamplesRgb = twoNearSamplesRgb * (1.0/2.0) + (1.0/4.0) * (
		texture2D(tex, textCoordinates.xy + edgeDir * (0.0/3.0 - 0.5)).xyz +
		texture2D(tex, textCoordinates.xy + edgeDir * (3.0/3.0 - 0.5)).xyz);
		
	float lumaFourSamples = dot(fourSamplesRgb, lumaFactor);
	
	if((lumaFourSamples < lumaMin) || (lumaFourSamples > lumaMax))
	{ //luma for 4 samples is too extrem (can be caused by end of edge): use only near samples
		fragColor = vec4(twoNearSamplesRgb, 1.0);
	} else
	{
		fragColor = vec4(fourSamplesRgb, 1.0);
	}
}
