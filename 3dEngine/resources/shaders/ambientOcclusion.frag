#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const uint KERNEL_SAMPLES = 64; //must be equals to AmbientOcclusionManager::KERNEL_SAMPLES_SHADER_LIMIT
layout(constant_id = 1) const float RADIUS = 0.0f;
layout(constant_id = 2) const float AO_STRENGTH = 0.0f;
layout(constant_id = 3) const float DEPTH_START_ATTENUATION = 0.0f;
layout(constant_id = 4) const float DEPTH_END_ATTENUATION = 0.0f;
layout(constant_id = 5) const uint NOISE_TEXTURE_SIZE = 0;
layout(constant_id = 6) const float BIAS = 0.0f;

layout(std140, set = 0, binding = 0) uniform PositioningData {
    mat4 mInverseViewProjection;
    mat4 mProjection;
    mat4 mView;
} positioningData;
layout(std140, set = 0, binding = 1) uniform KernelData {
    vec4 samples[KERNEL_SAMPLES];
} kernelData;
layout(std140, set = 0, binding = 2) uniform Scene {
    vec2 resolution;
} scene;
layout(binding = 3) uniform sampler2D depthTex;
layout(binding = 4) uniform sampler2D normalAndAmbientTex;
layout(binding = 5) uniform sampler2D noiseTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out float fragColor;

vec3 fetchEyePosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0f - 1.0f,
        texCoord.t * 2.0f - 1.0f,
        depthValue * 2.0f - 1.0f,
        1.0
    );
    vec4 position = inverse(positioningData.mProjection) * texPosition;
    position /= position.w;
    return vec3(position);
}

vec3 fetchPosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0f - 1.0f,
        texCoord.t * 2.0f - 1.0f,
        depthValue * 2.0f - 1.0f,
        1.0
    );
    vec4 position = positioningData.mInverseViewProjection * texPosition;
    position /= position.w;
    return vec3(position);
}

void main() {
    vec4 normalAndAmbient = vec4(texture(normalAndAmbientTex, texCoordinates));
    if (normalAndAmbient.a >= 0.99999f) { //no lighting
        fragColor = 0.0f;
        return;
    }

    float depthValue = texture(depthTex, texCoordinates).r;
    float distanceReduceFactor = 1.0f;
    if (depthValue > DEPTH_END_ATTENUATION) {
        fragColor = 0.0f;
        return;
    }else if (depthValue > DEPTH_START_ATTENUATION) {
        distanceReduceFactor = (DEPTH_END_ATTENUATION - depthValue) / (DEPTH_END_ATTENUATION - DEPTH_START_ATTENUATION);
    }

    vec3 position = fetchPosition(texCoordinates, depthValue);
    vec3 normal = normalAndAmbient.xyz * 2.0f - 1.0f;
    vec2 noiseScale = vec2(scene.resolution.x / NOISE_TEXTURE_SIZE, scene.resolution.y / NOISE_TEXTURE_SIZE);
    vec3 randomVector = normalize(texture(noiseTex, texCoordinates * noiseScale).xyz * 2.0f - 1.0f);

    vec3 tangent = normalize(randomVector - dot(randomVector, normal) * normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 kernelMatrix = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    for (int i = 0; i < KERNEL_SAMPLES; ++i) {
        vec3 sampleVectorWorldSpace = kernelMatrix * kernelData.samples[i].xyz;
        vec3 samplePointWorldSpace = position + RADIUS * sampleVectorWorldSpace;
        vec4 samplePointEyeSpace = positioningData.mView * vec4(samplePointWorldSpace, 1.0);
        vec4 samplePointClipSpace = positioningData.mProjection * samplePointEyeSpace;
        vec3 samplePointNDC = samplePointClipSpace.xyz / samplePointClipSpace.w;
        vec2 samplePointTexCoord = samplePointNDC.xy * 0.5f + 0.5f;

        float zSceneNDC = texture(depthTex, samplePointTexCoord).r;
        vec3 scenePositionEyeSpace = fetchEyePosition(samplePointTexCoord, zSceneNDC);

        float rangeCheck = smoothstep(0.0f, 1.0f, RADIUS / abs(scenePositionEyeSpace.z - samplePointEyeSpace.z));
        occlusion += (scenePositionEyeSpace.z >= samplePointEyeSpace.z + BIAS ? 1.0f : 0.0f) * rangeCheck;
    }

    fragColor = (occlusion / float(KERNEL_SAMPLES)) * distanceReduceFactor * AO_STRENGTH;

    //DEBUG: display noise texture
    //fragColor = texture(noiseTex, texCoordinates).x; //no repeat
    //fragColor = texture(noiseTex, texCoordinates * noiseScale).x; //repeat

    //DEBUG: display depth texture (pre-requisite: Renderer32#DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER must be activated)
    //fragColor = texture(depthTex, texCoordinates).r / 20.0f; //near objects are whiter

    //DEBUG: display normal texture (pre-requisite: Renderer32#DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER must be activated)
    //fragColor = texture(normalAndAmbientTex, texCoordinates).r; //normals to left are whiter
    //fragColor = texture(normalAndAmbientTex, texCoordinates).g; //normals to bottom are whiter
    //fragColor = texture(normalAndAmbientTex, texCoordinates).b; //normals to far are whiter

    //DEBUG: display kernel samples X values on X axis
    //fragColor = kernelData.samples[int(texCoordinates.x * KERNEL_SAMPLES)].x;
}
