#version 330

in vec2 textCoordinates;

uniform sampler2D normalTex;
uniform sampler2D dudvMap;
uniform float waveSpeed;
uniform float waveStrength;
uniform float sumTimeStep;
uniform vec3 waterColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main(){
    float speed = sumTimeStep * waveSpeed;
	vec2 distortedTexCoords = texture(dudvMap, vec2(textCoordinates.x + speed, textCoordinates.y)).rg * 0.1;
    distortedTexCoords = textCoordinates + vec2(distortedTexCoords.x, distortedTexCoords.y * speed);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    //diffuse
	fragColor = vec4(waterColor, 0.5);

	//normal and ambient factor
	vec2 normalTextCoordinates = textCoordinates + totalDistortion;
	vec4 normal = texture2D(normalTex, normalTextCoordinates);
	fragNormalAndAmbient = vec4(normal.xyz, 0.5);
}
