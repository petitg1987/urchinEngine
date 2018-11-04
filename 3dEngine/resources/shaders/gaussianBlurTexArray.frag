#version 440
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray tex;

in int gl_Layer;
in vec2 vertexTextCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = #OUTPUT_TYPE#(0.0);

	float weights[] = float[](#WEIGHTS_TAB#);
	float offsets[] = float[](#OFFSETS_TAB#);
	
	#LOOP1_START(#NB_TEXTURE_FETCH#)#		
		vec2 uvOffset = (#IS_VERTICAL_BLUR#) ? vec2(0.0, offsets[#LOOP1_COUNTER#]) : vec2(offsets[#LOOP1_COUNTER#], 0.0);
		fragColor += weights[#LOOP1_COUNTER#] 
			* texture2DArray(tex, vec3(vertexTextCoordinates+uvOffset, gl_Layer)).#SOURCE_TEX_COMPONENTS#;
	#LOOP1_END#
}