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

            void onLightAffectedZoneUpdated();
            void onSplitFrustumUpdated(const SplitFrustum&);

            void updateVisibleModels();

            const Matrix4<float>& getLightProjectionMatrix() const;
            float getSpotNearPlane() const;
            float getSpotFarPlane() const;

            std::span<Model* const> getModels() const;

        private:
            void updateSunLightScopeData(const SplitFrustum& splitFrustum);
            void updateSpotLightScopeData();

            float computeNearZForSceneIndependentBox(const Frustum<float>&) const;
            void stabilizeShadow(const Point3<float>&);
            Point2<float> computePixelCenteringDelta(const Matrix4<float>&, const Point4<float>&) const;
            Point4<float> adjustPointOnShadowMapPixel(const Matrix4<float>&, const Point4<float>&) const;
            std::pair<Point4<float>, Point3<float>> computeShadowTexCoords(const Matrix4<float>&, const Point4<float>&) const;

            const LightShadowMap* lightShadowMap;

            Matrix4<float> lightProjectionMatrix;
            Point4<float> previousCenter;
            float spotNearPlane;
            float spotFarPlane;
            std::unique_ptr<ConvexObject3D<float>> lightScopeConvexObject;

            std::vector<Model*> models;
    };

}
