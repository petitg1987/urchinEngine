#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "_transformFunctions.frag"

layout(constant_id = 0) const uint NUMBER_SHADOW_MAPS = 7; //must be equals to ShadowManager::SHADOW_MAPS_SHADER_LIMIT

layout (std140, push_constant) uniform PushConstants {
	int layerIndex;
} pushConstants;

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView; //not used
} postioningData;

layout(std140, set = 0, binding = 2) uniform ShadowData {
    mat4 lightProjectionViewMatrices[NUMBER_SHADOW_MAPS];
    float omnidirectionalNearPlane;
    float omnidirectionalFarPlane;
} shadowData;

layout(location = 0) in vec3 vertexPosition;

//instance data
layout(location = 1) in mat4 mModel; //use location 1, 2, 3 & 4

invariant gl_Position;

void main() {
    vec4 position = shadowData.lightProjectionViewMatrices[pushConstants.layerIndex] * (mModel * vec4(vertexPosition, 1.0));

    //depth linearization could be done directly via "shadowData.lightProjectionMatrix" but I didn't find how to do it !
    position.z = linearizeDepth(position.z / position.w, shadowData.omnidirectionalNearPlane, shadowData.omnidirectionalFarPlane) * position.w;

    gl_Position = position;
}
