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
#include <scene/renderer3d/camera/Camera.h>
#include <graphic/render/shader/model/Shader.h>

#define DEFAULT_NUMBER_SHADOW_MAPS 5
#define DEFAULT_SHADOW_MAP_RESOLUTION 1024
#define DEFAULT_VIEWING_SHADOW_DISTANCE 75.0
#define DEFAULT_BLUR_SHADOW BlurShadow::MEDIUM

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
                unsigned int nbShadowMaps = DEFAULT_NUMBER_SHADOW_MAPS;
                unsigned int shadowMapResolution = DEFAULT_SHADOW_MAP_RESOLUTION;
                float viewingShadowDistance = DEFAULT_VIEWING_SHADOW_DISTANCE;
                BlurShadow blurShadow = DEFAULT_BLUR_SHADOW;
            };

            ShadowManager(LightManager*, OctreeManager<Model>*);
            ~ShadowManager() override;

            void setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>&);
            void onCameraProjectionUpdate(const Camera*);
            void notify(Observable*, int) override;

            unsigned int getMaxShadowLights() const;
            float getShadowMapBias() const;

            void updateConfig(const Config&);
            const Config& getConfig() const;

            const std::vector<Frustum<float>>& getSplitFrustums() const;
            const LightShadowMap& getLightShadowMap(const Light*) const;

            void updateVisibleModels(const Frustum<float>&);
            const std::vector<Model*>& getVisibleModels();
            void removeModel(Model*);

            void forceUpdateAllShadowMaps();
            void updateShadowMaps();
            void loadShadowMaps(const std::shared_ptr<GenericRenderer>&, std::size_t);

        private:
            //light handling
            void addShadowLight(const Light*);
            void removeShadowLight(const Light*);
            void updateShadowLights();

            //splits handling
            void updateLightSplitsShadowMap(LightShadowMap*);
            void splitFrustum(const Frustum<float>&);

            //shadow map quality
            void checkConfig() const;
            static constexpr uint32_t SHADOW_MAPS_SHADER_LIMIT = 7; //must be equals to 'NUMBER_SHADOW_MAPS' in lighting shader
            const float shadowMapBias;
            const float percentageUniformSplit; //percentage of uniform split against the logarithmic split to split frustum
            Config config;

            //scene information
            LightManager* lightManager;
            OctreeManager<Model>* modelOctreeManager;
            Matrix4<float> projectionMatrix;
            std::vector<Model*> visibleModels;

            //shadow information
            std::vector<float> splitDistances;
            std::vector<Frustum<float>> splitFrustums;
            std::map<const Light*, LightShadowMap*> lightShadowMaps;
            bool bForceUpdateAllShadowMaps;
            float depthSplitDistance[SHADOW_MAPS_SHADER_LIMIT * 4]{}; //multiply by 4 because only 1 float over 4 are transferred to the shader due to memory alignment

            //shadow lights information
            Matrix4<float>* lightProjectionViewMatrices;
    };
}
