#version 330
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray tex;

in int gl_Layer;
in vec2 vertexTextCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = #OUTPUT_TYPE#(0.0);

	//TODO review it
	//TODO usage horizontal & vertical blur
	const float blurPixelSize = 1.0;
	const float resolution = 512.0; //TODO use uniform or token
	const float pixelSize = (1.0*blurPixelSize)/resolution;

    for(int s=-1; s<=1; s++){
        for(int t=-1; t<=1; t++){
            fragColor += texture2DArray(tex, vec3(vertexTextCoordinates.st + vec2(s, t)*pixelSize, gl_Layer)).#SOURCE_TEX_COMPONENTS#;
        }
    }
    
    fragColor /= 9.0;
}