#pragma once

#include <vector>
#include <map>
#include <memory>
#include <UrchinCommon.h>

#include "scene/renderer3d/lighting/shadow/SplitFrustum.h"
#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/lighting/light/spot/SpotLight.h"
#include "scene/renderer3d/lighting/light/LightManager.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/culler/ModelOcclusionCuller.h"
#include "scene/renderer3d/camera/Camera.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    /**
    * Manager for shadow mapping (parallel-split shadow maps & stabilized shadow maps)
    */
    class ShadowManager final : public Observer, public Observable {
        public:
            static constexpr unsigned int SPLIT_SHADOW_MAPS_SHADER_LIMIT = 6; //must be equals to 'MAX_SPLIT_SHADOW_MAPS' in lighting shaders
            static constexpr unsigned int SHADOW_MAP_OFFSET_TEX_SIZE = 10;

            enum NotificationType {
                NUMBER_SHADOW_MAPS_UPDATE
            };

            struct Config {
                unsigned int blurFilterBoxSize = 3;
                float shadowStrengthFactor = 1.0f;
                unsigned int nbSunShadowMaps = 5;
                unsigned int sunShadowMapResolution = 1024;
                float sunShadowViewDistance = 75.0f;
                float omniShadowMapResolutionFactor = 45.0f;
                unsigned int omniShadowMapMaxResolution = 1024;
                float spotShadowMapResolutionFactor = 40.0f;
                unsigned int spotShadowMapMaxResolution = 1024;
            };

            ShadowManager(const Config&, LightManager&, ModelOcclusionCuller&);
            ~ShadowManager() override = default;

            void setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t, uint32_t);
            void onCameraProjectionUpdate(const Camera&) const;
            void notify(Observable*, int) override;

            unsigned int getMaxShadowLights() const;
            unsigned int getMaxSplitShadowMaps() const;
            float getShadowMapConstantBiasFactor() const;
            float getShadowMapSlopeBiasFactor() const;
            unsigned int getShadowMapOffsetTexSize() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

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
            void addOrReplaceShadowLight(Light&);
            unsigned int computeShadowMapResolution(const OmnidirectionalLight&) const;
            unsigned int computeShadowMapResolution(const SpotLight&) const;
            void removeShadowLight(Light&);
            void updateShadowLights();

            //splits handling
            void updateSplitFrustum(const Frustum<float>&);

            //shadow map quality
            void checkConfig() const;
            Config config;

            //scene information
            LightManager& lightManager;
            ModelOcclusionCuller& modelOcclusionCuller;

            //shadow information
            std::vector<SplitFrustum> splitFrustums;
            std::map<const Light*, std::unique_ptr<LightShadowMap>> lightShadowMaps;
            std::shared_ptr<Texture> emptyShadowMapTexture;
            std::array<Point4<float>, (std::size_t)SPLIT_SHADOW_MAPS_SHADER_LIMIT> splitData;
            std::shared_ptr<Texture> shadowMapOffsetTexture;
            struct {
                alignas(4) int offsetSampleCount;
                alignas(4) float shadowStrengthFactor;
            } shadowMapInfo;

            //shadow lights information
            std::vector<Matrix4<float>> lightProjectionViewMatrices;
    };
}
