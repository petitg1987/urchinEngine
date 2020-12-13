#ifndef URCHINENGINE_FRUSTUMSHADOWDATA_H
#define URCHINENGINE_FRUSTUMSHADOWDATA_H

#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/data/ShadowData.h"
#include "scene/renderer3d/model/Model.h"

namespace urchin {

    /**
    * Shadow execution data for a light and a split frustum
    */
    class FrustumShadowData {
        public:
            explicit FrustumShadowData(ShadowData*);

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

            const ShadowData* shadowData;
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


