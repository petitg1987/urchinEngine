#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mView;
    vec3 cameraPosition;
    float sumTimeStep;
} positioningData;
layout(std140, set = 0, binding = 1) uniform GrassProperties {
    float displayDistance;
    float height;
    float grassLength;
    int numGrassInTex;
    float windStrength;
    vec3 windDirection;
} grassProperties;
layout(std140, set = 0, binding = 2) uniform TerrainPositioningData {
    vec3 minPoint;
    vec3 maxPoint;
} terrainPositioningData;
layout(std140, set = 0, binding = 3) uniform Projection {
    mat4 mProjection;
} projection;
layout(binding = 6) uniform sampler2D grassMaskTex;

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

layout(location = 0) in vec3 normal[];

layout(location = 0) smooth out vec2 vertexTexCoordinates;
layout(location = 1) out vec3 grassNormal;

float PHI = 1.61803398874989484820459 * 00000.1; //golden ratio
float PI  = 3.14159265358979323846264 * 00000.1; //PI
float SRT = 1.41421356237309504880169 * 10000.0; //square root of two

mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

float randZeroOne(in vec2 seed) {
    return fract(sin(dot(seed, vec2(PHI, PI)))*SRT);
}

int randomInt(int min, int max, vec3 seed) {
    vec2 seed2d = vec2(seed.x, seed.y + seed.z);
    float fRandomFloat = randZeroOne(seed2d);
    return int(round(float(min)+fRandomFloat*float(max-min)));
}

void main() {
    vec3 grassCenterPosition = gl_in[0].gl_Position.xyz;

    float s = (grassCenterPosition.x - terrainPositioningData.minPoint.x) / (terrainPositioningData.maxPoint.x - terrainPositioningData.minPoint.x);
    float t = (grassCenterPosition.z - terrainPositioningData.minPoint.z) / (terrainPositioningData.maxPoint.z - terrainPositioningData.minPoint.z);
    vec4 grassMask = texture(grassMaskTex, vec2(s, t));
    if (grassMask.x > 0.5) {
        return;
    }

    mat4 mProjectionView = projection.mProjection * positioningData.mView;
    grassNormal = normal[0];

    float grassYShift = 0.0f;
    float grassCameraDistance = distance(grassCenterPosition, positioningData.cameraPosition);
    float startReduceHeightDistance = grassProperties.displayDistance * 0.9f;
    if (grassCameraDistance > grassProperties.displayDistance) {
        return;
    }else if (grassCameraDistance > startReduceHeightDistance) {
        float grassHeightReducePercentage = (grassCameraDistance - startReduceHeightDistance) / (grassProperties.displayDistance - startReduceHeightDistance);
        grassYShift = - grassHeightReducePercentage * grassProperties.height;
    }

    float piOver180 = 3.14159f / 180.0f;
    vec3 baseDir[3];
    baseDir[0] = vec3(1.0, 0.0, 0.0);
    baseDir[1] = vec3(float(cos(45.0f * piOver180)), 0.0f, float(sin(45.0f * piOver180)));
    baseDir[2] = vec3(float(cos(-45.0f * piOver180)), 0.0f, float(sin(-45.0f * piOver180)));

    float windPower = 0.5f + sin(grassCenterPosition.x / 30.0f + grassCenterPosition.z / 30.0f + positioningData.sumTimeStep * (1.2f + grassProperties.windStrength / 20.0f));
    if (windPower > 0.0f) {
        windPower = windPower * 0.3f;
    }else{
        windPower = windPower * 0.2f;
    }
    windPower *= grassProperties.windStrength;

    float grassHalfLength = grassProperties.grassLength / 2.0f;
    for (int i = 0; i < 3; i++) {
        //texture selection
        vec3 seed = grassCenterPosition * float(i);
        float startTexX = float(randomInt(0, grassProperties.numGrassInTex - 1, seed)) * (1.0f / grassProperties.numGrassInTex);
        float endTexX = startTexX + 1.0f / grassProperties.numGrassInTex;

        //wind
        vec3 baseDirRotated = rotationMatrix(vec3(0, 1, 0), sin(positioningData.sumTimeStep * 0.7f) * 0.2f) * baseDir[i];

        //top left
        vec3 localTopLeft = grassCenterPosition - baseDirRotated * grassHalfLength + grassProperties.windDirection * windPower;
        localTopLeft.y += grassProperties.height + grassYShift;
        gl_Position = mProjectionView * vec4(localTopLeft, 1.0f);
        vertexTexCoordinates = vec2(startTexX, 0.0);
        EmitVertex();

        //bottom left
        vec3 localBottomLeft = grassCenterPosition - baseDir[i] * grassHalfLength;
        localBottomLeft.y += grassYShift;
        gl_Position = mProjectionView * vec4(localBottomLeft, 1.0f);
        vertexTexCoordinates = vec2(startTexX, 1.0);
        EmitVertex();

        //top right
        vec3 localTopRight = grassCenterPosition + baseDirRotated * grassHalfLength + grassProperties.windDirection * windPower;
        localTopRight.y += grassProperties.height + grassYShift;
        gl_Position = mProjectionView * vec4(localTopRight, 1.0f);
        vertexTexCoordinates = vec2(endTexX, 0.0);
        EmitVertex();

        //bottom right
        vec3 localBottomRight = grassCenterPosition + baseDir[i] * grassHalfLength;
        localBottomRight.y += grassYShift;
        gl_Position = mProjectionView * vec4(localBottomRight, 1.0f);
        vertexTexCoordinates = vec2(endTexX, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}
