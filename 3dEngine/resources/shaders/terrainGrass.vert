#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
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

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;

//instance data
layout(location = 2) in vec3 grassPosition;
layout(location = 3) in vec3 grassNormal;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 texCoordinates;
invariant gl_Position;

float randomZeroOne(in vec2 seed) {
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453123);
}

float randomFloat(float min, float max, vec3 seed) {
    vec2 seed2d = vec2(seed.x, seed.y + seed.z);
    float randomFloatZeroOne = randomZeroOne(seed2d);
    return min + randomFloatZeroOne * (max - min);
}

mat3 rotationMatrix(vec3 axis, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

void main() {
    normal = grassNormal;
    texCoordinates = texCoord;

    //initial grass rotation
    vec3 seed = grassPosition;
    float initialRotation = randomFloat(0.0, 6.283185307, seed); //from 0 to 2 * PI
    vec3 rotatedVertexPosition = rotationMatrix(vec3(0, 1, 0), initialRotation) * vertexPosition;

    vec3 grassGlobalPosition;
    if (texCoordinates.y < 0.5) { //top of the grass
        //wind: rotation
        float rotationSpeed = 0.7f;
        float rotationAngle = 0.2f;
        grassGlobalPosition = grassPosition + rotationMatrix(vec3(0, 1, 0), sin(positioningData.sumTimeStep * rotationSpeed) * rotationAngle) * rotatedVertexPosition;

        //wind: side
        float windPower = 0.5 + sin(grassGlobalPosition.x / 30.0 + grassGlobalPosition.z / 30.0 + positioningData.sumTimeStep * (1.2 + grassProperties.windStrength / 20.0));
        windPower *= (windPower > 0.0) ? 0.3 : 0.2;
        windPower *= grassProperties.windStrength;
        grassGlobalPosition += grassProperties.windDirection * windPower;

        //reduce grass height based on its distance from the camera
        float grassCameraDistance = distance(grassPosition, positioningData.cameraPosition);
        float startReduceHeightDistance = grassProperties.displayDistance * 0.9;
        if (grassCameraDistance > startReduceHeightDistance) {
            float grassHeightReducePercentage = 1.0f;
            if (grassCameraDistance < grassProperties.displayDistance) {
                grassHeightReducePercentage = (grassCameraDistance - startReduceHeightDistance) / (grassProperties.displayDistance - startReduceHeightDistance);
            }
            grassGlobalPosition.y -= grassHeightReducePercentage * grassProperties.height;
        }
    } else {
        grassGlobalPosition = grassPosition + rotatedVertexPosition;
    }

    gl_Position = positioningData.mProjectionView * vec4(grassGlobalPosition, 1.0);
}
