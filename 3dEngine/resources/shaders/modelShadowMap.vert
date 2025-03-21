#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const uint NUMBER_SHADOW_MAPS = 7; //must be equals to ShadowManager::SHADOW_MAPS_SHADER_LIMIT

layout (std140, push_constant) uniform PushConstants {
	int layerIndex;
} pushConstants;

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mProjectionView;
} postioningData;

layout(std140, set = 0, binding = 2) uniform ShadowData {
    mat4 matrices[NUMBER_SHADOW_MAPS];
} shadowData;

layout(location = 0) in vec3 vertexPosition;

//instance data
layout(location = 1) in mat4 mModel; //use location 1, 2, 3 & 4

invariant gl_Position;

void main() {
    vec4 position = shadowData.matrices[pushConstants.layerIndex] * (postioningData.mProjectionView * (mModel * vec4(vertexPosition, 1.0)));
    position.xyz /= position.w;
    position.w = 1.0f;
    gl_Position = position;
}
