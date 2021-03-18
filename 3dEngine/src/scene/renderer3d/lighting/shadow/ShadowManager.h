#ifndef URCHINENGINE_SHADOWMANAGER_H
#define URCHINENGINE_SHADOWMANAGER_H

#include <vector>
#include <map>
#include <memory>
#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/lighting/shadow/display/ShadowShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/lighting/light/LightManager.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/camera/Camera.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

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

            ShadowManager(LightManager*, OctreeManager<Model>*);
            ~ShadowManager() override;

            void initiateShaderVariables(const std::shared_ptr<Shader>&);
            void setupLightingRenderer(const std::unique_ptr<GenericRendererBuilder>&);
            void onCameraProjectionUpdate(const Camera*);
            void notify(Observable*, int) override;

            unsigned int getMaxShadowLights() const;
            float getShadowMapBias() const;

            void setShadowMapResolution(unsigned int);
            unsigned int getShadowMapResolution() const;
            void setNumberShadowMaps(unsigned int);
            unsigned int getNumberShadowMaps() const;
            void setViewingShadowDistance(float);
            float getViewingShadowDistance() const;
            void setBlurShadow(BlurShadow);
            BlurShadow getBlurShadow() const;

            const std::vector<Frustum<float>>& getSplitFrustums() const;
            const LightShadowMap& getLightShadowMap(const Light*) const;

            void updateVisibleModels(const Frustum<float>&);
            const std::vector<Model*>& getVisibleModels();
            void removeModel(Model*);

            void forceUpdateAllShadowMaps();
            void updateShadowMaps();
            void loadShadowMaps(const std::unique_ptr<GenericRenderer>&, std::size_t);

        private:
            //light handling
            void deleteLightsLocation();
            void addShadowLight(const Light*);
            void removeShadowLight(const Light*);
            void updateShadowLights();

            //splits handling
            void updateLightSplitsShadowMap(LightShadowMap*);
            void splitFrustum(const Frustum<float>&);

            //shadow map quality
            static constexpr uint32_t MAX_NB_SHADOW_MAPS = 20;
            const float shadowMapBias;
            const float percentageUniformSplit; //percentage of uniform split against the logarithmic split to split frustum
            unsigned int shadowMapResolution;
            unsigned int nbShadowMaps;
            float viewingShadowDistance;
            BlurShadow blurShadow;

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
            ShaderVar depthSplitDistanceShaderVar;
            float depthSplitDistance[MAX_NB_SHADOW_MAPS * 4]{}; //multiply by 4 because only 1 float over 4 are transferred to the shader due to memory alignment

            //shadow lights information
            ShaderVar** mLightProjectionViewShaderVar;
            Matrix4<float>* lightProjectionViewMatrices;
    };

}

#endif
