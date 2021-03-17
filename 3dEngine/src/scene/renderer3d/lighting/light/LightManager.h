#ifndef URCHINENGINE_LIGHTMANAGER_H
#define URCHINENGINE_LIGHTMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"
#include "Light.h"

namespace urchin {

    class LightManager : public Observable {
        public:
            explicit LightManager(std::shared_ptr<RenderTarget>);
            ~LightManager() override;

            enum NotificationType {
                ADD_LIGHT, //A light has been added
                REMOVE_LIGHT, //A light has been removed
            };

            void initiateShaderVariables(const std::shared_ptr<Shader>&);
            OctreeManager<Light>* getLightOctreeManager() const;
            Light* getLastUpdatedLight();

            unsigned int getMaxLights() const;
            const std::vector<Light*>& getVisibleLights() const;
            void addLight(Light*);
            void removeLight(Light*);

            void setGlobalAmbientColor(const Point4<float>&);
            const Point4<float>& getGlobalAmbientColor() const;

            void updateVisibleLights(const Frustum<float>&);
            void loadVisibleLights(const std::unique_ptr<GenericRenderer>&);
            void postUpdateVisibleLights();

            void drawLightOctree(const Matrix4<float>&, const Matrix4<float>&) const;

        private:
            void onLightEvent(Light*, NotificationType);
            void logMaxLightsReach();

            const unsigned int maxLights; //maximum of lights authorized to affect the scene in the same time
            std::shared_ptr<RenderTarget> renderTarget;

            //lights container
            std::vector<Light*> parallelBeamsLights; //sun lights
            OctreeManager<Light>* lightOctreeManager; //others lights
            std::vector<Light*> lightsInFrustum;
            std::vector<Light*> visibleLights;

            Light* lastUpdatedLight;

            struct LightShaderVar {
                ShaderVar isExistShaderVar;
                ShaderVar produceShadowShaderVar;
                ShaderVar hasParallelBeamsShaderVar;
                ShaderVar positionOrDirectionShaderVar;

                ShaderVar exponentialAttShaderVar;
                ShaderVar lightAmbientShaderVar;
            };
            LightShaderVar* lightsShaderVar;
            ShaderVar globalAmbientColorShaderVar;
            struct LightsData {
                alignas(4) bool isExist;
                alignas(4) bool produceShadow;
                alignas(4) bool hasParallelBeams;
                alignas(16) Vector3<float> positionOrDirection;
                alignas(4) float exponentialAttenuation;
                alignas(16) Point3<float> lightAmbient;
            };
            LightsData* lightsData;
            Point4<float> globalAmbientColor;
    };

}

#endif
