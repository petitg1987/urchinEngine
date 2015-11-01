#version 330

uniform sampler2D tex;

in vec2 vertexTextCoordinates;

layout (location = 0) out #OUTPUT_TYPE# fragColor;

void main(){
	fragColor = #OUTPUT_TYPE#(0.0);

	//TODO review in function of blurTexArray.frag

    for(int s=-1; s<=1; s++){
        for(int t=-1; t<=1; t++){
            fragColor += texture2D(tex, vertexTextCoordinates.st + vec2(s, t)).#SOURCE_TEX_COMPONENTS#;
        }
    }
    
    fragColor = fragColor/9.0;
}