#version 450

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

uniform mat4 mProjection;
uniform mat4 mView;
uniform vec3 cameraPosition;
uniform float sumTimeStep;
uniform vec3 terrainMinPoint;
uniform vec3 terrainMaxPoint;
uniform sampler2D grassMaskTex;
uniform float grassDisplayDistance;
uniform float grassHeight;
uniform float grassHalfLength;
uniform int numGrassInTex;
uniform vec3 windDirection;
uniform float windStrength;

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
    return int(round(float(min)+fRandomFloat*float(max-min)));
}

void main(){

    vec3 grassCenterPosition = gl_in[0].gl_Position.xyz;

    float s = (grassCenterPosition.x - terrainMinPoint.x) / (terrainMaxPoint.x - terrainMinPoint.x);
    float t = (grassCenterPosition.z - terrainMinPoint.z) / (terrainMaxPoint.z - terrainMinPoint.z);
    vec4 grassMask = texture2D(grassMaskTex, vec2(s, t));
    if(grassMask.x > 0.5){
        return;
    }

    mat4 mProjectionView = mProjection * mView;
    grassNormal = normal[0];

    float grassYShift = 0.0f;
    float grassCameraDistance = distance(grassCenterPosition, cameraPosition);
    float startReduceHeightDistance = grassDisplayDistance * 0.9f;
    if(grassCameraDistance > grassDisplayDistance)
    {
        return;
    }else if(grassCameraDistance > startReduceHeightDistance)
    {
        float grassHeightReducePercentage = (grassCameraDistance - startReduceHeightDistance) / (grassDisplayDistance - startReduceHeightDistance);
        grassYShift = - grassHeightReducePercentage * grassHeight;
    }

    float piOver180 = 3.14159f/180.0f;
    vec3 baseDir[3];
    baseDir[0] = vec3(1.0, 0.0, 0.0);
    baseDir[1] = vec3(float(cos(45.0f*piOver180)), 0.0f, float(sin(45.0f*piOver180)));
    baseDir[2] = vec3(float(cos(-45.0f*piOver180)), 0.0f, float(sin(-45.0f*piOver180)));

    float windPower = 0.5f + sin(grassCenterPosition.x/30.0f + grassCenterPosition.z/30.0f + sumTimeStep*(1.2f+windStrength/20.0f));
    if(windPower > 0.0f){
        windPower = windPower*0.3f;
    }else{
        windPower = windPower*0.2f;
    }
    windPower *= windStrength;

    for(int i = 0; i < 3; i++)
    {
        //texture selection
        vec3 seed = grassCenterPosition * float(i);
        float startTextX = float(randomInt(0, numGrassInTex-1, seed)) * (1.0f / numGrassInTex);
        float endTextX = startTextX + 1.0f / numGrassInTex;

        //wind
        vec3 baseDirRotated = rotationMatrix(vec3(0, 1, 0), sin(sumTimeStep*0.7f)*0.2f) * baseDir[i];

        //top left
        vec3 localTopLeft = grassCenterPosition - baseDirRotated*grassHalfLength + windDirection*windPower;
        localTopLeft.y += grassHeight + grassYShift;
        gl_Position = mProjectionView * vec4(localTopLeft, 1.0f);
        vertexTextCoordinates = vec2(startTextX, 0.0);
        EmitVertex();

        //bottom left
        vec3 localBottomLeft = grassCenterPosition - baseDir[i]*grassHalfLength;
        localBottomLeft.y += grassYShift;
        gl_Position = mProjectionView * vec4(localBottomLeft, 1.0f);
        vertexTextCoordinates = vec2(startTextX, 1.0);
        EmitVertex();

        //top right
        vec3 localTopRight = grassCenterPosition + baseDirRotated*grassHalfLength + windDirection*windPower;
        localTopRight.y += grassHeight + grassYShift;
        gl_Position = mProjectionView * vec4(localTopRight, 1.0f);
        vertexTextCoordinates = vec2(endTextX, 0.0);
        EmitVertex();

        //bottom right
        vec3 localBottomRight = grassCenterPosition + baseDir[i]*grassHalfLength;
        localBottomRight.y += grassYShift;
        gl_Position = mProjectionView * vec4(localBottomRight, 1.0f);
        vertexTextCoordinates = vec2(endTextX, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}
