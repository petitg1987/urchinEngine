#version 330
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray tex;

in int gl_Layer;
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
			* texture2DArray(tex, vec3(vertexTextCoordinates+offseti, gl_Layer)).#SOURCE_TEX_COMPONENTS#;
	#LOOP1_END#
}