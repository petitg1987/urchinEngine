#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/SplitFrustum.h>
#include <scene/renderer3d/model/Model.h>

namespace urchin {

    class LightSplitShadowMap {
        public:
            static constexpr float LIGHT_BOX_MARGIN = 0.05f;

            explicit LightSplitShadowMap(const LightShadowMap*);

            void update(const SplitFrustum&);

            const AABBox<float> &getShadowCasterReceiverBox() const;
            const Matrix4<float>& getLightProjectionMatrix() const;
            std::span<Model* const> getModels() const;

        private:
            void computeLightScope(const SplitFrustum&, const Matrix4<float>&);
            float computeNearZForSceneIndependentBox(const Frustum<float>&) const;

            void stabilizeShadow(const Point3<float>&);
            Point2<float> computePixelCenteringDelta(const Matrix4<float>&, const Point4<float>&) const;
            Point4<float> adjustPointOnShadowMapPixel(const Matrix4<float>&, const Point4<float>&) const;
            std::pair<Point4<float>, Point3<float>> computeShadowTexCoords(const Matrix4<float>&, const Point4<float>&) const;

            const LightShadowMap* lightShadowMap;

            Matrix4<float> lightProjectionMatrix;
            AABBox<float> shadowCasterReceiverBox;
            Point4<float> previousCenter;

            std::vector<Model*> models;
    };

}
