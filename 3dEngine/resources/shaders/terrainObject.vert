#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
    vec3 cameraPosition;
    float sumTimeStep;
} positioningData;

layout(std140, set = 0, binding = 1) uniform MeshData {
    uint lightMask;
    float encodedEmissiveFactor; //encoded between 0.0 (no emissive) and 1.0 (max emissive)
    float ambientFactor;
    float modelHeight;
    float modelMinY;
} meshData;

layout(std140, set = 0, binding = 3) uniform Properties {
    float displayDistance;
    bool useTerrainLighting;
    float windStrength;
    vec3 windDirection;
} properties;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

//instance data
layout(location = 4) in mat4 mModel; //use location 4, 5, 6 & 7
layout(location = 8) in mat4 mNormal; //use location 8, 9, 10 & 11
layout(location = 12) in vec3 terrainNormal; //use location 12

layout(location = 0) out vec3 t;
layout(location = 1) out vec3 b;
layout(location = 2) out vec3 n;
layout(location = 3) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    if (properties.useTerrainLighting) {
        t = vec3(0.0, 0.0, 0.0);
        n = terrainNormal;
        b = vec3(0.0, 0.0, 0.0);
    } else {
        t = normalize(mat3(mNormal) * tangent);
        n = normalize(mat3(mNormal) * normal);
        b = normalize(cross(n, t));
    }

    vec4 position = mModel * vec4(vertexPosition, 1.0);
    float localVertexHeight = vertexPosition.y - meshData.modelMinY;

    //wind
    if (properties.windStrength > 0.00001f) {
        //the higher the vertex is on the model, the stronger the wind becomes
        float globalWindStrength = localVertexHeight / meshData.modelHeight;

        //rotation
        float rotationSpeed = 1.0f;
        float rotationStrength = 0.05f * globalWindStrength;
        position.x += sin(positioningData.sumTimeStep * rotationSpeed) * rotationStrength;
        position.z += cos(positioningData.sumTimeStep * rotationSpeed) * rotationStrength;

        //side
        float windPower = 0.5 + sin(position.x / 30.0 + position.z / 30.0 + positioningData.sumTimeStep * (1.2 + properties.windStrength / 20.0));
        windPower *= (windPower > 0.0) ? 0.3 : 0.2;
        windPower *= properties.windStrength;
        windPower *= globalWindStrength;
        position.xyz += properties.windDirection * windPower;
    }

    //reduce height based on its distance from the camera
    vec3 positionVec3 = position.xyz / position.w;
    float distanceToCamera = distance(positionVec3, positioningData.cameraPosition);
    float startReduceHeightDistance = properties.displayDistance * 0.75;
    if (distanceToCamera > startReduceHeightDistance) {
        float heightReducePercentage = 1.0;
        if (distanceToCamera < properties.displayDistance) {
            heightReducePercentage = (distanceToCamera - startReduceHeightDistance) / (properties.displayDistance - startReduceHeightDistance);
        }
        float scaleY = mModel[1][1];
        position.y -= heightReducePercentage * localVertexHeight * scaleY;
    }

    gl_Position = positioningData.mProjectionView * position;
}
