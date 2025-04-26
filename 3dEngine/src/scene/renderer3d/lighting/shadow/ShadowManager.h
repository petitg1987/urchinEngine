#pragma once

#include <vector>
#include <map>
#include <memory>
#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/SplitFrustum.h>
#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/light/Light.h>
#include <scene/renderer3d/lighting/light/LightManager.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/camera/Camera.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    /**
    * Manager for shadow mapping (parallel-split shadow maps & stabilized shadow maps)
    */
    class ShadowManager final : public Observer, public Observable {
        public:
            enum NotificationType {
                NUMBER_SHADOW_MAPS_UPDATE
            };

            struct Config {
                unsigned int blurFilterBoxSize = 3;
                float shadowStrengthFactor = 1.0f;
                unsigned int nbSunShadowMaps = 5;
                unsigned int sunShadowMapResolution = 1024;
                float sunShadowViewDistance = 75.0f;
                float spotShadowMapResolutionFactor = 20.0f;
                unsigned int spotShadowMapMaxResolution = 1024;
            };

            ShadowManager(const Config&, LightManager&, ModelOcclusionCuller&);
            ~ShadowManager() override = default;

            void setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t, uint32_t);
            void onCameraProjectionUpdate(const Camera&);
            void notify(Observable*, int) override;

            unsigned int getMaxShadowLights() const;
            float getShadowMapConstantBias() const;
            float getShadowMapSlopeBiasFactor() const;
            unsigned int getShadowMapOffsetTexSize() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            const std::vector<SplitFrustum>& getSplitFrustums() const;
            const std::shared_ptr<Texture>& getEmptyShadowMapTexture() const;
            const LightShadowMap& getLightShadowMap(const Light*) const;

            void updateVisibleModels(const Frustum<float>&);
            void removeModel(Model*) const;

            void updateShadowMaps(uint32_t, unsigned int) const;
            void loadShadowMaps(GenericRenderer&, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

        private:
            //global shadow handling
            void updateShadowMapOffsets();

            //light handling
            void addShadowLight(Light&);
            void removeShadowLight(const Light&);
            void updateShadowLights();

            //splits handling
            void updateLightSplitsShadowMap(const LightShadowMap&) const;
            void splitFrustum(const Frustum<float>&);

            //shadow map quality
            void checkConfig() const;
            static constexpr unsigned int SHADOW_MAPS_SHADER_LIMIT = 7; //must be equals to 'NUMBER_SHADOW_MAPS' in lighting shader
            static constexpr float SHADOW_MAP_CONSTANT_BIAS = 0.00008f;
            static constexpr float SHADOW_MAP_SLOPE_BIAS_FACTOR = 0.0011f;
            static constexpr unsigned int SHADOW_MAP_OFFSET_TEX_SIZE = 10;
            Config config;

            //scene information
            LightManager& lightManager;
            ModelOcclusionCuller& modelOcclusionCuller;

            //shadow information
            std::vector<SplitFrustum> splitFrustums;
            std::map<const Light*, std::unique_ptr<LightShadowMap>> lightShadowMaps;
            std::shared_ptr<Texture> emptyShadowMapTexture;
            std::array<Point4<float>, (std::size_t)SHADOW_MAPS_SHADER_LIMIT> splitData;
            std::shared_ptr<Texture> shadowMapOffsetTexture;
            struct {
                alignas(4) int offsetSampleCount;
                alignas(4) float shadowStrengthFactor;
            } shadowMapInfo;

            //shadow lights information
            std::vector<Matrix4<float>> lightProjectionViewMatrices;
    };
}
