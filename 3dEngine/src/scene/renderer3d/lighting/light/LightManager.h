#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <resources/geometry/aabbox/AABBoxModel.h>
#include <scene/renderer3d/lighting/light/Light.h>

namespace urchin {

    class LightManager : public Observable {
        public:
            explicit LightManager(RenderTarget&);
            ~LightManager() override;

            enum NotificationType {
                ADD_LIGHT, //A light has been added
                REMOVE_LIGHT, //A light has been removed
            };

            void setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>&);
            OctreeManager<Light>* getLightOctreeManager() const;
            Light* getLastUpdatedLight();

            unsigned int getMaxLights() const;
            const std::vector<Light*>& getSunLights() const;
            const std::vector<Light*>& getVisibleLights() const;
            void addLight(Light*);
            void removeLight(Light*);

            void setGlobalAmbientColor(const Point4<float>&);
            const Point4<float>& getGlobalAmbientColor() const;

            void updateVisibleLights(const Frustum<float>&);
            void loadVisibleLights(GenericRenderer&);
            void postUpdateVisibleLights();

            void drawLightOctree(const Matrix4<float>&, const Matrix4<float>&);

        private:
            void onLightEvent(Light*, NotificationType);
            void logMaxLightsReach();

            static const unsigned int LIGHTS_SHADER_LIMIT;

            const unsigned int maxLights; //maximum of lights authorized to affect the scene in the same time
            RenderTarget& renderTarget;

            //lights container
            std::vector<Light*> sunLights;
            OctreeManager<Light>* lightOctreeManager; //all lights except sun lights
            std::unique_ptr<AABBoxModel> debugLightOctree;
            std::vector<Light*> lightsInFrustum;
            std::vector<Light*> visibleLights;

            Light* lastUpdatedLight;

            struct LightsData {
                alignas(4) bool isExist;
                alignas(4) bool produceShadow;
                alignas(4) bool hasParallelBeams;
                alignas(16) Vector3<float> positionOrDirection;
                alignas(4) float exponentialAttenuation;
                alignas(16) Point3<float> lightAmbient;
            };
            std::vector<LightsData> lightsData;
            Point4<float> globalAmbientColor;
    };

}
