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
            explicit LightManager(const RenderTarget*);
            ~LightManager() override;

            enum NotificationType {
                ADD_LIGHT, //A light has been added
                REMOVE_LIGHT, //A light has been removed
            };

            void initiateShaderVariables(const std::unique_ptr<Shader>&);
            OctreeManager<Light>* getLightOctreeManager() const;
            Light* getLastUpdatedLight();

            unsigned int getMaxLights() const;
            const std::vector<Light*>& getVisibleLights() const;
            void addLight(Light*);
            void removeLight(Light*);

            void setGlobalAmbientColor(const Point4<float>&);
            const Point4<float>& getGlobalAmbientColor() const;

            void updateLights(const Frustum<float>&);
            void loadLights();
            void postUpdateLights();

            void drawLightOctree(const Matrix4<float>&, const Matrix4<float>&) const;

        private:
            void onLightEvent(Light*, NotificationType);
            void checkMaxLight(const std::vector<Light*>&) const;

            const RenderTarget* renderTarget;

            //lights container
            std::vector<Light*> parallelBeamsLights; //sun lights
            OctreeManager<Light>* lightOctreeManager; //others lights
            std::vector<Light*> lightsInFrustum;
            std::vector<Light*> visibleLights;

            Light* lastUpdatedLight;

            const unsigned int maxLights; //maximum of lights authorized to affect the scene in the same time
            struct LightInfo {
                ShaderVar isExistShaderVar;
                ShaderVar produceShadowShaderVar;
                ShaderVar hasParallelBeamsShaderVar;
                ShaderVar positionOrDirectionShaderVar;

                ShaderVar exponentialAttShaderVar;
                ShaderVar lightAmbientShaderVar;
            };
            LightInfo* lightsInfo;

            ShaderVar globalAmbientColorShaderVar;
            Point4<float> globalAmbientColor;
    };

}

#endif
