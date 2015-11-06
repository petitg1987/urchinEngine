#version 330
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray tex;

in int gl_Layer;
in vec2 vertexTextCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = #OUTPUT_TYPE#(0.0);

	//TODO review it and use uniform
	const float blurPixelSize = 1.0;
	const float resolution = 4096.0;
	const float pixelSize = (1.0*blurPixelSize)/resolution;

	float blurFactors[3][3];
	blurFactors[0][0] = 1;
	blurFactors[0][1] = 2;
	blurFactors[0][2] = 1;
	blurFactors[1][0] = 2;
	blurFactors[1][1] = 4;
	blurFactors[1][2] = 2;
	blurFactors[2][0] = 1;
	blurFactors[2][1] = 2;
	blurFactors[2][2] = 1;

    for(int s=-1; s<=1; s++){
        for(int t=-1; t<=1; t++){
            fragColor += blurFactors[s+1][t+1] * texture2DArray(tex, vec3(vertexTextCoordinates.st + vec2(s, t)*pixelSize, gl_Layer)).#SOURCE_TEX_COMPONENTS#;
        }
    }
    
    fragColor /= 16.0;
}