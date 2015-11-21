#version 330

uniform sampler2D tex;

in vec2 vertexTextCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = #OUTPUT_TYPE#(0.0);

	float weights[] = float[](#WEIGHTS_TAB#);
	float offsets[] = float[](#OFFSETS_TAB#);
	
	vec2 offseti;
	#LOOP1_START(#NB_TEXTURE_FETCH#)#
		if(#IS_VERTICAL_GAUSSIAN#){
			offseti = vec2(0.0, offsets[#LOOP1_COUNTER#]);
		}else{
			offseti = vec2(offsets[#LOOP1_COUNTER#], 0.0);
		}
		
		fragColor += weights[#LOOP1_COUNTER#] 
			* texture2D(tex, vertexTextCoordinates+offseti).#SOURCE_TEX_COMPONENTS#;
	#LOOP1_END#
}