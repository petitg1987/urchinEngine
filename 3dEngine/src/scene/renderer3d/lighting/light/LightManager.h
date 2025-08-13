#pragma once

#include <vector>
#include <UrchinCommon.h>

#include "graphics/api/GraphicsApi.h"
#include "graphics/render/GenericRendererBuilder.h"
#include "resources/geometry/aabbox/AABBoxModel.h"
#include "resources/geometry/GeometryContainer.h"
#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    class LightManager final : public Observable {
        public:
            LightManager();

            enum NotificationType {
                ADD_LIGHT, //A light has been added
                REMOVE_LIGHT, //A light has been removed
            };

            void setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t) const;
            OctreeManager<Light>& getLightOctreeManager();
            Light* getLastUpdatedLight() const;

            unsigned int getMaxLights() const;
            const std::vector<std::shared_ptr<Light>>& getSunLights() const;
            std::span<Light* const> getVisibleLights() const;
            void addLight(std::shared_ptr<Light>);
            std::shared_ptr<Light> removeLight(Light*);

            void setGlobalAmbientColor(const Point3<float>&);
            const Point3<float>& getGlobalAmbientColor() const;

            void updateVisibleLights(const Frustum<float>&);
            void loadVisibleLights(GenericRenderer&, uint32_t, const std::vector<Light*>&);
            void postUpdateVisibleLights();

            void drawLightOctree(GeometryContainer&);

        private:
            void onLightEvent(Light*, NotificationType);

            static constexpr unsigned int MAX_LIGHTS = 1000; //must be equals to MAX_LIGHTS in deferredSecondPass/modelTransparent shaders

            //lights container
            std::vector<std::shared_ptr<Light>> sunLights;
            OctreeManager<Light> lightOctreeManager; //all lights except sun
            std::shared_ptr<AABBoxModel> debugLightOctree;
            std::vector<Light*> lightsInFrustum;
            std::vector<Light*> visibleLights;

            Light* lastUpdatedLight;

            struct LightInfo {
                alignas(4) int lightType;
                alignas(4) unsigned int lightMask;
                alignas(4) bool isPbrEnabled;
                alignas(4) bool hasShadow;
                alignas(4) int shadowLightIndex;
                alignas(4) float shadowStrength;
                alignas(16) Point3<float> lightColor;
                alignas(16) Vector3<float> direction;
                alignas(16) Point3<float> position;
                alignas(4) float exponentialAttenuation;
                alignas(4) float innerCosAngle;
                alignas(4) float outerCosAngle;
            };

            struct LightsData { //Warning: any changes in this struct required to review 'dataUpdateSize' in 'loadVisibleLights' method
                alignas(16) Point3<float> globalAmbientColor;
                alignas(4) unsigned int lightsCount;
                alignas(16) std::array<LightInfo, MAX_LIGHTS> lightsInfo;
            } lightsData;
    };

}
