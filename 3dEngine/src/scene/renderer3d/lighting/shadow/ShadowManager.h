#pragma once

#include <vector>
#include <map>
#include <memory>
#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h>
#include <scene/renderer3d/lighting/light/Light.h>
#include <scene/renderer3d/lighting/light/LightManager.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/camera/Camera.h>
#include <graphics/api/vulkan/render/shader/Shader.h>

namespace urchin {

    /**
    * Manager for shadow mapping (parallel-split shadow maps and variance shadow maps)
    */
    class ShadowManager : public Observer, public Observable {
        public:
            enum BlurShadow {
                NO_BLUR = 0,
                LOW = 3,
                MEDIUM = 5,
                HIGH = 7
            };

            enum NotificationType {
                NUMBER_SHADOW_MAPS_UPDATE
            };

            struct Config {
                unsigned int nbShadowMaps = 5;
                unsigned int shadowMapResolution = 1024;
                float viewingShadowDistance = 75.0;
                BlurShadow blurShadow = BlurShadow::MEDIUM;
            };

            ShadowManager(const Config&, LightManager&, ModelOcclusionCuller&);
            ~ShadowManager() override = default;

            void setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>&);
            void onCameraProjectionUpdate(const Camera&);
            void notify(Observable*, int) override;

            unsigned int getMaxShadowLights() const;
            float getShadowMapConstantBias() const;
            float getShadowMapSlopeBiasFactor() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            const std::vector<Frustum<float>>& getSplitFrustums() const;
            const std::shared_ptr<Texture>& getEmptyShadowMapTexture() const;
            const LightShadowMap& getLightShadowMap(const Light*) const;

            void updateVisibleModels(const Frustum<float>&);
            std::span<Model* const> getVisibleModels() const;
            void removeModel(Model*) const;

            void updateShadowMaps(std::uint32_t, unsigned int) const;
            void loadShadowMaps(GenericRenderer&, std::size_t);

        private:
            //light handling
            void addShadowLight(Light&);
            void removeShadowLight(Light&);
            void updateShadowLights();

            //splits handling
            void updateLightSplitsShadowMap(const LightShadowMap&);
            void splitFrustum(const Frustum<float>&);

            //shadow map quality
            void checkConfig() const;
            static constexpr uint32_t SHADOW_MAPS_SHADER_LIMIT = 7; //must be equals to 'NUMBER_SHADOW_MAPS' in lighting shader
            static constexpr float SHADOW_MAP_CONSTANT_BIAS = 0.0002f;
            static constexpr float SHADOW_MAP_SLOPE_BIAS_FACTOR = 0.0005f;
            Config config;

            //scene information
            LightManager& lightManager;
            ModelOcclusionCuller& modelOcclusionCuller;
            Matrix4<float> projectionMatrix;
            std::vector<Model*> visibleModels;

            //shadow information
            std::vector<float> splitDistances;
            std::vector<Frustum<float>> splitFrustums;
            std::map<Light*, std::unique_ptr<LightShadowMap>> lightShadowMaps;
            std::shared_ptr<Texture> emptyShadowMapTexture;
            std::array<float, SHADOW_MAPS_SHADER_LIMIT * 4> depthSplitDistance; //multiply by 4 because only 1 float over 4 are transferred to the shader due to memory alignment

            //shadow lights information
            std::vector<Matrix4<float>> lightProjectionViewMatrices;
    };
}
