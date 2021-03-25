#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define KERNEL_SAMPLES 0
#define RADIUS 0
#define BIAS 0
#define AO_STRENGTH 0
#define NOISE_TEXTURE_SIZE 0
#define DEPTH_START_ATTENUATION 0
#define DEPTH_END_ATTENUATION 0

uniform mat4 mInverseViewProjection; //binding 0
uniform mat4 mProjection; //binding 0
uniform mat4 mView; //binding 0
uniform vec4 samples[KERNEL_SAMPLES]; //binding 1
uniform vec2 resolution; //binding 2
layout(binding = 20) uniform sampler2D depthTex;
layout(binding = 21) uniform sampler2D normalAndAmbientTex;
layout(binding = 22) uniform sampler2D noiseTex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out float fragColor;

vec3 fetchEyePosition(vec2 texCoord, float depthValue) {
    vec4 texPosition = vec4(
        texCoord.s * 2.0f - 1.0f,
        texCoord.t * 2.0f - 1.0f,
        depthValue * 2.0f - 1.0f,
        1.0
    );
    vec4 position = inverse(mProjection) * texPosition;
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
    vec4 position = mInverseViewProjection * texPosition;
    position /= position.w;
    return vec3(position);
}

void main() {
    vec4 normalAndAmbient = vec4(texture2D(normalAndAmbientTex, texCoordinates));
    if (normalAndAmbient.a >= 0.99999f) { //no lighting
        fragColor = 0.0f;
        return;
    }

    float depthValue = texture2D(depthTex, texCoordinates).r;
    float distanceReduceFactor = 1.0f;
    if (depthValue > DEPTH_END_ATTENUATION) {
        fragColor = 0.0f;
        return;
    }else if (depthValue > DEPTH_START_ATTENUATION) {
        distanceReduceFactor = (DEPTH_END_ATTENUATION - depthValue) / (DEPTH_END_ATTENUATION - DEPTH_START_ATTENUATION);
    }

    vec3 position = fetchPosition(texCoordinates, depthValue);
    vec3 normal = normalAndAmbient.xyz * 2.0f - 1.0f;
    vec2 noiseScale = vec2(resolution.x / NOISE_TEXTURE_SIZE, resolution.y / NOISE_TEXTURE_SIZE);
    vec3 randomVector = normalize(texture(noiseTex, texCoordinates * noiseScale).xyz * 2.0f - 1.0f);

    vec3 tangent = normalize(randomVector - dot(randomVector, normal) * normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 kernelMatrix = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    for (int i = 0; i < KERNEL_SAMPLES; ++i) {
        vec3 sampleVectorWorldSpace = kernelMatrix * samples[i].xyz;
        vec3 samplePointWorldSpace = position + RADIUS * sampleVectorWorldSpace;
        vec4 samplePointEyeSpace = mView * vec4(samplePointWorldSpace, 1.0);
        vec4 samplePointClipSpace = mProjection * samplePointEyeSpace;
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
    //fragColor = texture2D(depthTex, texCoordinates).r / 20.0f; //near objects are whiter

    //DEBUG: display normal texture (pre-requisite: Renderer32#DEBUG_DISPLAY_AMBIENT_OCCLUSION_BUFFER must be activated)
    //fragColor = texture2D(normalAndAmbientTex, texCoordinates).r; //normals to left are whiter
    //fragColor = texture2D(normalAndAmbientTex, texCoordinates).g; //normals to bottom are whiter
    //fragColor = texture2D(normalAndAmbientTex, texCoordinates).b; //normals to far are whiter

    //DEBUG: display kernel samples X values on X axis
    //fragColor = samples[int(texCoordinates.x * KERNEL_SAMPLES)].x;
}
