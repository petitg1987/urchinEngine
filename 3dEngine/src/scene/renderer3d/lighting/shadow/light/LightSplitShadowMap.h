#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/model/Model.h>

namespace urchin {

    class LightSplitShadowMap {
        public:
            static constexpr float LIGHT_BOX_MARGIN = 0.05f;

            explicit LightSplitShadowMap(const LightShadowMap*);

            void update(const Frustum<float>&);

            const AABBox<float> &getShadowCasterReceiverBox() const;
            const Matrix4<float>& getLightProjectionMatrix() const;
            std::span<Model* const> getModels() const;

        private:
            AABBox<float> createSceneIndependentBox(const Frustum<float>&, const Matrix4<float>&) const;
            float computeNearZForSceneIndependentBox(const Frustum<float>&) const;
            AABBox<float> buildSceneDependentBox(const AABBox<float>&, const OBBox<float>&) const;
            void updateShadowCasterReceiverBox(const AABBox<float>&);
            Point2<float> stabilizeShadows(const Matrix4<float>&, const Point4<float>&) const;
            Point4<float> stabilizeShadows2(const Matrix4<float>&, const Point4<float>&) const;
            Point3<float> computeRoundedShadowTexCoord(const Matrix4<float>&, const Point4<float>&, Point4<float>&) const;

            const LightShadowMap* lightShadowMap;

            const bool useSceneDependentProjection;
            Matrix4<float> lightProjectionMatrix;
            AABBox<float> shadowCasterReceiverBox;
            Point4<float> previousCenter;

            std::vector<Model*> models;
    };

}
