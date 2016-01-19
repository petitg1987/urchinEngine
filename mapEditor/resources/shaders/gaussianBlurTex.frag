#version 330

uniform sampler2D tex;

in vec2 textCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = #OUTPUT_TYPE#(0.0);

	float weights[] = float[](#WEIGHTS_TAB#);
	float offsets[] = float[](#OFFSETS_TAB#);

	#LOOP1_START(#NB_TEXTURE_FETCH#)#		
		vec2 uvOffset = (#IS_VERTICAL_BLUR#) ? vec2(0.0, offsets[#LOOP1_COUNTER#]) : vec2(offsets[#LOOP1_COUNTER#], 0.0);
		fragColor += weights[#LOOP1_COUNTER#] 
			* texture2D(tex, textCoordinates+uvOffset).#SOURCE_TEX_COMPONENTS#;
	#LOOP1_END#
}