#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <resources/geometry/GeometryContainer.h>
#include <scene/renderer3d/lighting/light/Light.h>

namespace urchin {

    class LightManager : public Observable {
        public:
            LightManager();

            enum NotificationType {
                ADD_LIGHT, //A light has been added
                REMOVE_LIGHT, //A light has been removed
            };

            void setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>&) const;
            OctreeManager<Light>& getLightOctreeManager();
            Light* getLastUpdatedLight();

            unsigned int getMaxLights() const;
            const std::vector<std::shared_ptr<Light>>& getSunLights() const;
            std::span<Light* const> getVisibleLights() const;
            void addLight(std::shared_ptr<Light>);
            std::shared_ptr<Light> removeLight(Light*);

            void setGlobalAmbientColor(const Point3<float>&);
            const Point3<float>& getGlobalAmbientColor() const;

            void updateVisibleLights(const Frustum<float>&);
            void loadVisibleLights(GenericRenderer&, std::size_t);
            void postUpdateVisibleLights();

            void drawLightOctree(GeometryContainer&);

        private:
            void onLightEvent(Light*, NotificationType);
            void logMaxLightsReach() const;

            static constexpr unsigned int MAX_LIGHTS = 10; //maximum of lights authorized to affect the scene in the same time
            static constexpr unsigned int LIGHTS_SHADER_LIMIT = 15; //must be equals to MAX_LIGHTS/LIGHTS_SHADER_LIMIT/MAX_LIGHTS in lighting/modelShadowMap/modelTransparent shaders

            //lights container
            std::vector<std::shared_ptr<Light>> sunLights;
            OctreeManager<Light> lightOctreeManager; //all lights except sunµ
            std::shared_ptr<AABBoxModel> debugLightOctree;
            std::vector<Light*> lightsInFrustum;
            std::vector<Light*> visibleLights;

            Light* lastUpdatedLight;

            struct LightInfo {
                alignas(4) bool isExist;
                alignas(4) bool produceShadow;
                alignas(4) bool hasParallelBeams;
                alignas(16) Vector3<float> positionOrDirection;
                alignas(4) float exponentialAttenuation;
                alignas(16) Point3<float> lightColor;
            };

            struct LightsData {
                alignas(16) std::array<LightInfo, LIGHTS_SHADER_LIMIT> lightsInfo;
                alignas(16) Point3<float> globalAmbientColor;
            };
            LightsData lightsData;
    };

}
