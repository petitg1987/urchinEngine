#ifndef URCHINENGINE_SHADOWMANAGER_H
#define URCHINENGINE_SHADOWMANAGER_H

#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/data/ShadowData.h"
#include "scene/renderer3d/lighting/shadow/display/ShadowShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/lighting/light/LightManager.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/ModelDisplayer.h"
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

            void initiateShaderVariables(const std::unique_ptr<Shader>&);
            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera*);
            void notify(Observable*, int) override;

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
            const ShadowData& getShadowData(const Light*) const;
            const std::vector<Model*>& computeVisibleModels();

            void updateVisibleModels(const Frustum<float>&);
            void forceUpdateAllShadowMaps();
            void updateShadowMaps();
            void loadShadowMaps(const std::unique_ptr<GenericRenderer>&);

            void drawLightSceneBox(const RenderTarget*, const Frustum<float>&, const Light*, const Matrix4<float>&) const;

        private:
            //model displayer
            void createOrUpdateShadowModelDisplayer();

            //light handling
            void deleteLightsLocation();
            void addShadowLight(const Light*);
            void removeShadowLight(const Light*);
            void updateShadowLights();

            //splits handling
            void updateViewMatrix(const Light*);
            void updateFrustumShadowData(const Light*, ShadowData*);
            AABBox<float> createSceneIndependentBox(const Frustum<float>&, const Matrix4<float>&) const;
            float computeNearZForSceneIndependentBox(const Frustum<float>&) const;
            AABBox<float> createSceneDependentBox(const AABBox<float>&, const OBBox<float>&,
                    const std::vector<Model*>&, const Matrix4<float>&) const;
            void splitFrustum(const Frustum<float>&);

            //shadow map handling
            void createShadowMaps(const Light*);

            //shadow map quality
            const float shadowMapBias;
            const float percentageUniformSplit; //percentage of uniform split against the logarithmic split to split frustum
            float lightViewOverflowStepSize;
            unsigned int shadowMapResolution;
            unsigned int nbShadowMaps;
            float viewingShadowDistance;
            BlurShadow blurShadow;

            //scene information
            unsigned int sceneWidth, sceneHeight;
            ModelDisplayer *shadowModelDisplayer;
            LightManager *lightManager;
            OctreeManager<Model> *modelOctreeManager;
            std::vector<Model*> obboxModels;
            Matrix4<float> projectionMatrix;
            ShadowShaderVariable *shadowShaderVariable;
            ShadowModelShaderVariable *shadowModelShaderVariable;
            std::vector<Model*> visibleModels;

            //shadow information
            TextureFormat depthTextureFormat;
            float frustumDistance;
            std::vector<float> splitDistances;
            std::vector<Frustum<float>> splitFrustums;
            std::map<const Light*, ShadowData*> shadowDatas;
            bool bForceUpdateAllShadowMaps;
            ShaderVar depthSplitDistanceShaderVar;

            //light information
            struct LightLocation { //reservation of light locations for light producing shadow
                ShaderVar shadowMapTexShaderVar;
                ShaderVar *mLightProjectionViewShaderVar = nullptr;
            };
            LightLocation *lightsLocation;
    };

}

#endif
