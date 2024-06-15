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

void main() {
    normal = grassNormal;
    texCoordinates = texCoord;

    vec3 grassGlobalPosition = grassPosition + vertexPosition;

    if (texCoordinates.y < 0.5) { //top of the grass
        float grassCameraDistance = distance(grassPosition, positioningData.cameraPosition);
        float startReduceHeightDistance = grassProperties.displayDistance * 0.9;
        if (grassCameraDistance > startReduceHeightDistance) {
            float grassHeightReducePercentage = 1.0f;
            if (grassCameraDistance < grassProperties.displayDistance) {
                grassHeightReducePercentage = (grassCameraDistance - startReduceHeightDistance) / (grassProperties.displayDistance - startReduceHeightDistance);
            }
            grassGlobalPosition.y -= grassHeightReducePercentage * grassProperties.height;
        }

        float windPower = 0.5 + sin(grassGlobalPosition.x / 30.0 + grassGlobalPosition.z / 30.0 + positioningData.sumTimeStep * (1.2 + grassProperties.windStrength / 20.0));
        if (windPower > 0.0) {
            windPower = windPower * 0.3;
        } else {
            windPower = windPower * 0.2;
        }
        windPower *= grassProperties.windStrength;

        grassGlobalPosition += grassProperties.windDirection * windPower;
    }

    gl_Position = positioningData.mProjectionView * vec4(grassGlobalPosition, 1.0);
}
