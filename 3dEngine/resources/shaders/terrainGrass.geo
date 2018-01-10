#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

uniform mat4 mProjection;
uniform mat4 mView;
uniform float sumTimeStep;
uniform vec3 terrainMinPoint;
uniform vec3 terrainMaxPoint;
uniform sampler2D grassMaskTex;

in vec3 normal[];

smooth out vec2 vertexTextCoordinates;
out vec3 grassNormal;

float PHI = 1.61803398874989484820459 * 00000.1; //golden ratio
float PI  = 3.14159265358979323846264 * 00000.1; //PI
float SRT = 1.41421356237309504880169 * 10000.0; //square root of two

mat3 rotationMatrix(vec3 axis, float angle){
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

float randZeroOne(in vec2 seed){
    return fract(sin(dot(seed, vec2(PHI, PI)))*SRT);
}

int randomInt(int min, int max, vec3 seed){
    vec2 seed2d = vec2(seed.x, seed.y + seed.z);
	float fRandomFloat = randZeroOne(seed2d);
	return int(float(min)+fRandomFloat*float(max-min));
}

void main(){

    vec3 grassCenterPosition = gl_in[0].gl_Position.xyz;

    float s = (grassCenterPosition.x - terrainMinPoint.x) / (terrainMaxPoint.x - terrainMinPoint.x);
    float t = (grassCenterPosition.z - terrainMinPoint.z) / (terrainMaxPoint.z - terrainMinPoint.z);
    vec4 grassMask = texture2D(grassMaskTex, vec2(s, t));
    if(grassMask.x > 0.5){
        return;
    }

    float halfLengthGrass = 0.6f; //TODO configurable
    float heightGrass = 0.4f; //TODO configurable
    int nbGrassTexture = 1; //TODO configurable

    mat4 mProjectionView = mProjection * mView;

    float PiOver180 = 3.14159f/180.0f;
    vec3 vBaseDir[3];
    vBaseDir[0] = vec3(1.0, 0.0, 0.0);
    vBaseDir[1] = vec3(float(cos(45.0f*PiOver180)), 0.0f, float(sin(45.0f*PiOver180)));
    vBaseDir[2] = vec3(float(cos(-45.0f*PiOver180)), 0.0f, float(sin(-45.0f*PiOver180)));

	vec3 vWindDirection = vec3(0.707, 0.0, 0.707); //TODO configurable
	float fWindStrength = 0.6f; //TODO configurable
    float fWindPower = 0.5f + sin(grassCenterPosition.x/30.0f + grassCenterPosition.z/30.0f + sumTimeStep*(1.2f+fWindStrength/20.0f));
    if(fWindPower > 0.0f){
        fWindPower = fWindPower*0.3f;
    }else{
        fWindPower = fWindPower*0.2f;
    }
    fWindPower *= fWindStrength;

    grassNormal = normal[0];

    for(int i = 0; i < 3; i++)
	{
	    //texture selection
        vec3 seed = grassCenterPosition * float(i);
        float startTextX = float(randomInt(0, nbGrassTexture-1, seed)) * (1.0f / nbGrassTexture);
        float endTextX = startTextX + 1.0f / nbGrassTexture;

        //wind
        vec3 vBaseDirRotated = rotationMatrix(vec3(0, 1, 0), sin(sumTimeStep*0.7f)*0.2f) * vBaseDir[i];

        //top left
        vec3 localTopLeft = grassCenterPosition - vBaseDirRotated*halfLengthGrass + vWindDirection*fWindPower;
        localTopLeft.y += heightGrass;
        gl_Position = mProjectionView * vec4(localTopLeft, 1.0f);
        vertexTextCoordinates = vec2(startTextX, 0.0);
        EmitVertex();

        //bottom left
        vec3 localBottomLeft = grassCenterPosition - vBaseDir[i]*halfLengthGrass;
        gl_Position = mProjectionView * vec4(localBottomLeft, 1.0f);
        vertexTextCoordinates = vec2(startTextX, 1.0);
        EmitVertex();

        //top right
        vec3 localTopRight = grassCenterPosition + vBaseDirRotated*halfLengthGrass + vWindDirection*fWindPower;
        localTopRight.y += heightGrass;
        gl_Position = mProjectionView * vec4(localTopRight, 1.0f);
        vertexTextCoordinates = vec2(endTextX, 0.0);
        EmitVertex();

        //bottom right
        vec3 localBottomRight = grassCenterPosition + vBaseDir[i]*halfLengthGrass;
        gl_Position = mProjectionView * vec4(localBottomRight, 1.0f);
        vertexTextCoordinates = vec2(endTextX, 1.0);
        EmitVertex();

        EndPrimitive();
	}
}
