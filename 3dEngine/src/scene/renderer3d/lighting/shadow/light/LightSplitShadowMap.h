#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/SplitFrustum.h>
#include <scene/renderer3d/model/Model.h>

namespace urchin {

    class LightSplitShadowMap {
        public:
            static constexpr float LIGHT_BOX_MARGIN = 0.05f;

            explicit LightSplitShadowMap(unsigned int, const LightShadowMap*);

            void onLightAffectedZoneUpdated();
            void onSplitFrustumUpdated(const SplitFrustum&);

            void updateVisibleModels();

            const Matrix4<float>& getLightProjectionViewMatrix() const;

            std::span<Model* const> getModels() const;

        private:
            void updateLightViewMatrix();
            void updateSunLightScopeData(const SplitFrustum& splitFrustum);
            void updateOmnidirectionalLightScopeData();
            void updateSpotLightScopeData();

            float computeNearZForSceneIndependentBox(const Frustum<float>&) const;
            void stabilizeShadow(const Point3<float>&);
            Point2<float> computePixelCenteringDelta(const Matrix4<float>&, const Point4<float>&) const;
            Point4<float> adjustPointOnShadowMapPixel(const Matrix4<float>&, const Point4<float>&) const;
            std::pair<Point4<float>, Point3<float>> computeShadowTexCoords(const Matrix4<float>&, const Point4<float>&) const;

            unsigned int splitIndex;
            const LightShadowMap* lightShadowMap;

            Matrix4<float> lightProjectionMatrix;
            Matrix4<float> lightViewMatrix;
            Matrix4<float> lightProjectionViewMatrix;
            Point4<float> previousCenter;
            std::unique_ptr<ConvexObject3D<float>> lightScopeConvexObject;

            std::vector<Model*> models;
    };

}
