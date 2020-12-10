#ifndef URCHINENGINE_FRUSTUMSHADOWDATA_H
#define URCHINENGINE_FRUSTUMSHADOWDATA_H

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"

namespace urchin {

    /**
    * Shadow execution data for a light and a split frustum
    */
    class FrustumShadowData {
        public:
            FrustumShadowData();

            void updateShadowCasterReceiverBox(const AABBox<float>&, bool);
            const Matrix4<float>& getLightProjectionMatrix() const;

            void updateModels(const std::vector<Model*>&);
            const std::vector<Model*>& getModels() const;

            bool needShadowMapUpdate() const;

        private:
            bool areAlmostIdenticalAABBox(const AABBox<float>&, const AABBox<float>&) const;

            const float updateShadowMapThreshold;
            Matrix4<float> lightProjectionMatrix;
            AABBox<float> shadowCasterReceiverBox;
            bool shadowCasterReceiverBoxUpdated;

            std::vector<Model*> models;
            bool modelsRequireUpdate;
    };

}

#endif


