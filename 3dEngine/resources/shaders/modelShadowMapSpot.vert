#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const uint MAX_SPLIT_SHADOW_MAPS = 6; //must be equals to ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView; //not used
} postioningData;

layout(std140, set = 0, binding = 2) uniform ShadowData {
    mat4 lightProjectionViewMatrix;
} shadowData;

layout(location = 0) in vec3 vertexPosition;

//instance data
layout(location = 1) in mat4 mModel; //use location 1, 2, 3 & 4

invariant gl_Position;

void main() {
    gl_Position = shadowData.lightProjectionViewMatrix * (mModel * vec4(vertexPosition, 1.0));
}
