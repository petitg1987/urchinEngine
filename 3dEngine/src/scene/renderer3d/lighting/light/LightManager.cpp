#include <stdexcept>

#include "LightManager.h"
#include "scene/renderer3d/lighting/light/sun/SunLight.h"
#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/utils/OctreeRenderer.h"
#include "graphic/shader/data/ShaderDataSender.h"

#define DEFAULT_OCTREE_MIN_SIZE 50.0f

namespace urchin {

    LightManager::LightManager(const RenderTarget* renderTarget) :
            renderTarget(renderTarget),
            lightOctreeManager(new OctreeManager<Light>(DEFAULT_OCTREE_MIN_SIZE)),
            lastUpdatedLight(nullptr),
            maxLights(ConfigService::instance()->getUnsignedIntValue("light.maxLights")),
            lightsInfo(new LightInfo[maxLights]),
            globalAmbientColor(Point4<float>(0.0, 0.0, 0.0, 0.0)) {

    }

    LightManager::~LightManager() {
        std::vector<Light*> allOctreeableLights = lightOctreeManager->getAllOctreeables();
        for (auto allOctreeableLight : allOctreeableLights) {
            delete allOctreeableLight;
        }
        for (auto& parallelBeamsLight : parallelBeamsLights) {
            delete parallelBeamsLight;
        }

        delete lightOctreeManager;
        delete[] lightsInfo;
    }

    void LightManager::initiateShaderVariables(const std::unique_ptr<Shader>& lightingShader) {
        std::ostringstream isExistLocName, produceShadowLocName, hasParallelBeamsName, positionOrDirectionLocName;
        std::ostringstream exponentialAttName, lightAmbientName;
        for (unsigned int i=0;i<maxLights;++i) {
            isExistLocName.str("");
            isExistLocName << "lightsInfo[" << i << "].isExist";
            lightsInfo[i].isExistShaderVar = ShaderVar(lightingShader, isExistLocName.str());

            produceShadowLocName.str("");
            produceShadowLocName << "lightsInfo[" << i << "].produceShadow";
            lightsInfo[i].produceShadowShaderVar = ShaderVar(lightingShader, produceShadowLocName.str());

            hasParallelBeamsName.str("");
            hasParallelBeamsName << "lightsInfo[" << i << "].hasParallelBeams";
            lightsInfo[i].hasParallelBeamsShaderVar = ShaderVar(lightingShader, hasParallelBeamsName.str());

            positionOrDirectionLocName.str("");
            positionOrDirectionLocName << "lightsInfo[" << i << "].positionOrDirection";
            lightsInfo[i].positionOrDirectionShaderVar = ShaderVar(lightingShader, positionOrDirectionLocName.str());

            exponentialAttName.str("");
            exponentialAttName << "lightsInfo[" << i << "].exponentialAttenuation";
            lightsInfo[i].exponentialAttShaderVar = ShaderVar(lightingShader, exponentialAttName.str());

            lightAmbientName.str("");
            lightAmbientName << "lightsInfo[" << i << "].lightAmbient";
            lightsInfo[i].lightAmbientShaderVar = ShaderVar(lightingShader, lightAmbientName.str());
        }

        globalAmbientColorShaderVar = ShaderVar(lightingShader, "globalAmbient");
    }

    OctreeManager<Light>* LightManager::getLightOctreeManager() const {
        return lightOctreeManager;
    }

    void LightManager::onLightEvent(Light* light, NotificationType notificationType) {
        lastUpdatedLight = light;
        notifyObservers(this, notificationType);
    }

    Light* LightManager::getLastUpdatedLight() {
        return lastUpdatedLight;
    }

    /**
     * @return Maximum of lights authorized to affect the scene in the same time
     */
    unsigned int LightManager::getMaxLights() const {
        return maxLights;
    }

    const std::vector<Light*>& LightManager::getVisibleLights() const {
        return visibleLights;
    }

    void LightManager::addLight(Light* light) {
        if (light) {
            if (light->hasParallelBeams()) {
                parallelBeamsLights.push_back(light);
            } else {
                lightOctreeManager->addOctreeable(light);
            }

            onLightEvent(light, LightManager::ADD_LIGHT);
        }
    }

    void LightManager::removeLight(Light* light) {
        if (light) {
            if (light->hasParallelBeams()) {
                auto it = std::find(parallelBeamsLights.begin(), parallelBeamsLights.end(), light);
                parallelBeamsLights.erase(it);
            } else {
                lightOctreeManager->removeOctreeable(light);
            }

            onLightEvent(light, LightManager::REMOVE_LIGHT);

            delete light;
        }
    }

    void LightManager::setGlobalAmbientColor(const Point4<float>& globalAmbientColor) {
        this->globalAmbientColor = globalAmbientColor;
    }

    const Point4<float>& LightManager::getGlobalAmbientColor() const {
        return globalAmbientColor;
    }

    void LightManager::updateLights(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "updateLights");

        lightOctreeManager->refreshOctreeables();
        lightsInFrustum.clear();
        lightOctreeManager->getOctreeablesIn(frustum, lightsInFrustum);

        visibleLights.clear();
        visibleLights = parallelBeamsLights;
        visibleLights.insert(visibleLights.end(), lightsInFrustum.begin(), lightsInFrustum.end());
    }

    void LightManager::loadLights() {
        const std::vector<Light*>& lights = getVisibleLights();
        checkMaxLight(lights);

        for (unsigned int i=0; i < maxLights; ++i) {
            if (lights.size() > i) {
                const Light* light = lights[i];

                ShaderDataSender()
                    .sendData(lightsInfo[i].isExistShaderVar, true)
                    .sendData(lightsInfo[i].produceShadowShaderVar, light->isProduceShadow())
                    .sendData(lightsInfo[i].hasParallelBeamsShaderVar, light->hasParallelBeams())
                    .sendData(lightsInfo[i].lightAmbientShaderVar, light->getAmbientColor());

                if (lights[i]->getLightType() == Light::SUN) {
                    const auto* sunLight = dynamic_cast<const SunLight*>(light);
                    ShaderDataSender().sendData(lightsInfo[i].positionOrDirectionShaderVar, sunLight->getDirections()[0]);
                } else if (lights[i]->getLightType() == Light::OMNIDIRECTIONAL) {
                    const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light);
                    ShaderDataSender()
                        .sendData(lightsInfo[i].positionOrDirectionShaderVar, omnidirectionalLight->getPosition())
                        .sendData(lightsInfo[i].exponentialAttShaderVar, omnidirectionalLight->getExponentialAttenuation());
                } else {
                    throw std::invalid_argument("Unknown light type to load shader variables: " + std::to_string(light->getLightType()));
                }
            } else {
                ShaderDataSender().sendData(lightsInfo[i].isExistShaderVar, false);
                break;
            }
        }

        ShaderDataSender().sendData(globalAmbientColorShaderVar, getGlobalAmbientColor());
    }

    void LightManager::checkMaxLight(const std::vector<Light*>& lights) const {
        static bool maxLightReachLogged = false;
        if (lights.size() > maxLights && !maxLightReachLogged) {
            Logger::instance()->logWarning("Light in scene (" + std::to_string(lights.size()) + ") is higher that max light (" + std::to_string(maxLights) + ") authorized");
            maxLightReachLogged = true;
        }
    }

    void LightManager::postUpdateLights() {
        lightOctreeManager->postRefreshOctreeables();
    }

    void LightManager::drawLightOctree(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        OctreeRenderer::drawOctree(lightOctreeManager, renderTarget, projectionMatrix, viewMatrix);
    }

}
