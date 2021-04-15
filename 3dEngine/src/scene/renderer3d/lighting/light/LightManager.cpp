#include <stdexcept>

#include "LightManager.h"
#include "scene/renderer3d/lighting/light/sun/SunLight.h"
#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/util/OctreeRenderer.h"

#define DEFAULT_OCTREE_MIN_SIZE 50.0f

namespace urchin {

    //static
    constexpr unsigned int LightManager::LIGHTS_SHADER_LIMIT = 15; //must be equals to 'MAX_LIGHTS'/'MAX_VERTICES' in lighting/modelShadowMap shaders

    LightManager::LightManager(std::shared_ptr<RenderTarget> renderTarget) :
            maxLights(ConfigService::instance()->getUnsignedIntValue("light.maxLights")),
            renderTarget(std::move(renderTarget)),
            lightOctreeManager(new OctreeManager<Light>(DEFAULT_OCTREE_MIN_SIZE)),
            lastUpdatedLight(nullptr),
            lightsData(nullptr),
            globalAmbientColor(Point4<float>(0.0f, 0.0f, 0.0f, 0.0f)) {
        if(maxLights > LIGHTS_SHADER_LIMIT) {
            throw std::invalid_argument("Maximum lights value is limited to " + std::to_string(LIGHTS_SHADER_LIMIT));
        }
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
        delete[] lightsData;
    }

    void LightManager::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        delete[] lightsData;
        std::size_t lightsDataSize = maxLights;
        lightsData = new LightsData[lightsDataSize];

        lightingRendererBuilder
                ->addShaderData(lightsDataSize * sizeof(LightsData), lightsData) //binding 2
                ->addShaderData(sizeof(globalAmbientColor), &globalAmbientColor); //binding 3
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

    void LightManager::updateVisibleLights(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "updateLights");

        lightOctreeManager->refreshOctreeables();
        lightsInFrustum.clear();
        lightOctreeManager->getOctreeablesIn(frustum, lightsInFrustum);

        visibleLights.clear();
        visibleLights = parallelBeamsLights;
        visibleLights.insert(visibleLights.end(), lightsInFrustum.begin(), lightsInFrustum.end());

        if (visibleLights.size() > maxLights) {
            logMaxLightsReach();
            visibleLights.resize(maxLights);
        }

        /*
         * Sort visible lights by their memory address.
         * By sorting visible lights, we make sure that the lights producing shadow are often in same order.
         * Thus, the shadow textures positions/index in shader don't need to be updated at each frame in ShadowManager#loadShadowMaps().
         */
        sort(visibleLights.begin(), visibleLights.end(), std::greater<>());
    }

    void LightManager::loadVisibleLights(const std::shared_ptr<GenericRenderer>& lightingRenderer) {
        const std::vector<Light*>& lights = getVisibleLights();

        for (unsigned int i = 0; i < maxLights; ++i) {
            if (lights.size() > i) {
                const Light* light = lights[i];

                lightsData[i].isExist = true;
                lightsData[i].produceShadow = light->isProduceShadow();
                lightsData[i].hasParallelBeams = light->hasParallelBeams();
                lightsData[i].lightAmbient = light->getAmbientColor();

                if (lights[i]->getLightType() == Light::SUN) {
                    const auto* sunLight = dynamic_cast<const SunLight*>(light);
                    lightsData[i].positionOrDirection = sunLight->getDirections()[0];
                } else if (lights[i]->getLightType() == Light::OMNIDIRECTIONAL) {
                    const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light);
                    lightsData[i].positionOrDirection = omnidirectionalLight->getPosition().toVector();
                    lightsData[i].exponentialAttenuation = omnidirectionalLight->getExponentialAttenuation();
                } else {
                    throw std::invalid_argument("Unknown light type to load shader variables: " + std::to_string(light->getLightType()));
                }
            } else {
                lightsData[i].isExist = false;
                break;
            }
        }

        lightingRenderer->updateShaderData(2, lightsData);
        lightingRenderer->updateShaderData(3, &globalAmbientColor);
    }

    void LightManager::postUpdateVisibleLights() {
        lightOctreeManager->postRefreshOctreeables();
    }

    void LightManager::logMaxLightsReach() {
        static bool maxLightReachLogged = false;
        if (!maxLightReachLogged) {
            Logger::instance()->logWarning("Light in scene (" + std::to_string(visibleLights.size()) + ") is higher that max light (" + std::to_string(maxLights) + ") authorized");
            maxLightReachLogged = true;
        }
    }

    void LightManager::drawLightOctree(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        OctreeRenderer::drawOctree(lightOctreeManager, renderTarget, projectionMatrix, viewMatrix);
    }

}
