#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

uniform mat4 mProjection;
uniform mat4 mView;
uniform float sumTimeStep;

smooth out vec2 vertexTextCoordinates;
out vec3 normal;

vec3 vLocalSeed; //TODO don't use global var

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

float randZeroOne()
{ //TODO simplify ?
    uint n = floatBitsToUint(vLocalSeed.y * 214013.0 + vLocalSeed.x * 2531011.0 + vLocalSeed.z * 141251.0);
    n = n * (n * n * 15731u + 789221u);
    n = (n >> 9u) | 0x3F800000u;

    float fRes =  2.0 - uintBitsToFloat(n);
    vLocalSeed = vec3(vLocalSeed.x + 147158.0 * fRes, vLocalSeed.y*fRes  + 415161.0 * fRes, vLocalSeed.z + 324154.0*fRes);
    return fRes;
}

int randomInt(int min, int max)
{
	float fRandomFloat = randZeroOne();
	return int(float(min)+fRandomFloat*float(max-min));
}

void main(){

    mat4 mProjectionView = mProjection * mView;

    float PiOver180 = 3.14159f/180.0f;
    vec3 vBaseDir[3];
    vBaseDir[0] = vec3(1.0, 0.0, 0.0);
    vBaseDir[1] = vec3(float(cos(45.0f*PiOver180)), 0.0f, float(sin(45.0f*PiOver180)));
    vBaseDir[2] = vec3(float(cos(-45.0f*PiOver180)), 0.0f, float(sin(-45.0f*PiOver180)));

    vec3 grassCenterPosition = gl_in[0].gl_Position.xyz;
    float halfLengthGrass = 0.6f;
    float heightGrass = 0.2f;

	vec3 vWindDirection = vec3(0.707, 0.0, 0.707);
	float fWindStrength = 0.6f;

    for(int i = 0; i < 3; i++)
	{
	    //texture selection
        vLocalSeed = grassCenterPosition * float(i);
        float startTextX = 0.0f; //float(randomInt(0, 3)) * 0.25f;
        float endTextX = 1.0f; //startTextX+0.25f;

        //wind (Y axis) //TODO is it really Y axis rotation ?
        vec3 vBaseDirRotated = (rotationMatrix(vec3(0, 1, 0), sin(sumTimeStep*0.7f)*0.2f) * vec4(vBaseDir[i], 1.0)).xyz; //TODO use mat 4 & vec3 ?

        //wind
        float fWindPower = 0.5f+sin(grassCenterPosition.x/30+grassCenterPosition.z/30+sumTimeStep*(1.2f+fWindStrength/20.0f)); //TODO pre-compute when possible
        if(fWindPower < 0.0f){
        	fWindPower = fWindPower*0.2f;
        }else{
            fWindPower = fWindPower*0.3f;
        }
        fWindPower *= fWindStrength;

        //TODO use terrain normal
        normal = vec3(0.5, 1.0, 0.5);

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
