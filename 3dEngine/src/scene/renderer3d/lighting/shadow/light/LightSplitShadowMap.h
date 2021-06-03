#ifndef URCHINENGINE_LIGHTSPLITSHADOWMAP_H
#define URCHINENGINE_LIGHTSPLITSHADOWMAP_H

#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/model/Model.h>

namespace urchin {

    class LightSplitShadowMap {
        public:
            static const float LIGHT_BOX_MARGIN;

            explicit LightSplitShadowMap(LightShadowMap*);

            void update(const Frustum<float>&, bool);

            const AABBox<float> &getShadowCasterReceiverBox() const;
            const Matrix4<float>& getLightProjectionMatrix() const;
            const std::vector<Model*>& getModels() const;
            bool needShadowMapUpdate() const;

        private:
            AABBox<float> createSceneIndependentBox(const Frustum<float>&, const Matrix4<float>&) const;
            float computeNearZForSceneIndependentBox(const Frustum<float>&) const;
            void buildSceneDependentBox(const AABBox<float>&, const OBBox<float>&, bool);
            void updateShadowCasterReceiverBox(const AABBox<float>&, bool);
            bool areAlmostIdenticalAABBox(const AABBox<float>&, const AABBox<float>&) const;
            void updateModels(const std::vector<Model*>&);

            const LightShadowMap* lightShadowMap;
            std::vector<Model*> obboxModels;

            const float updateShadowMapThreshold;
            Matrix4<float> lightProjectionMatrix;
            AABBox<float> shadowCasterReceiverBox;
            bool shadowCasterReceiverBoxUpdated;

            std::vector<Model*> models;
            bool modelsRequireUpdate;
    };

}

#endif


