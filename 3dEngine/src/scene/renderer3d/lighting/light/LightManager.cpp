#include <stdexcept>

#include <scene/renderer3d/lighting/light/LightManager.h>
#include <scene/renderer3d/lighting/light/sun/SunLight.h>
#include <scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

namespace urchin {

    //static
    constexpr unsigned int LightManager::LIGHTS_SHADER_LIMIT = 15; //must be equals to 'MAX_LIGHTS'/'MAX_VERTICES' in lighting/modelShadowMap shaders
    constexpr float LightManager::LIGHTS_OCTREE_MIN_SIZE = 50.0f;

    LightManager::LightManager(RenderTarget& renderTarget) :
            maxLights(ConfigService::instance().getUnsignedIntValue("light.maxLights")),
            renderTarget(renderTarget),
            lightOctreeManager(std::make_unique<OctreeManager<Light>>(LIGHTS_OCTREE_MIN_SIZE)),
            lastUpdatedLight(nullptr),
            globalAmbientColor(Point4<float>(0.0f, 0.0f, 0.0f, 0.0f)) {
        if (maxLights > LIGHTS_SHADER_LIMIT) {
            throw std::invalid_argument("Maximum lights value is limited to " + std::to_string(LIGHTS_SHADER_LIMIT));
        }
    }

    void LightManager::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        std::size_t lightsDataSize = maxLights;

        LightsData emptyLightData{};
        lightsData.resize(lightsDataSize, emptyLightData);

        lightingRendererBuilder
                ->addUniformData(lightsDataSize * sizeof(LightsData), lightsData.data()) //binding 2
                ->addUniformData(sizeof(globalAmbientColor), &globalAmbientColor); //binding 3
    }

    OctreeManager<Light>& LightManager::getLightOctreeManager() const {
        return *lightOctreeManager;
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

    const std::vector<std::shared_ptr<Light>>& LightManager::getSunLights() const {
        return sunLights;
    }

    const std::vector<Light*>& LightManager::getVisibleLights() const {
        return visibleLights;
    }

    void LightManager::addLight(std::shared_ptr<Light> light) {
        if (light) {
            Light* lightPtr = light.get();
            if (light->hasParallelBeams()) {
                sunLights.push_back(std::move(light));
            } else {
                lightOctreeManager->addOctreeable(std::move(light));
            }

            onLightEvent(lightPtr, LightManager::ADD_LIGHT);
        }
    }

    std::shared_ptr<Light> LightManager::removeLight(Light* light) {
        std::shared_ptr<Light> removedLight;
        if (light) {
            if (light->hasParallelBeams()) {
                for (auto it = sunLights.begin(); it != sunLights.end(); ++it) {
                    if (it->get() == light) {
                        removedLight = std::move(*it);
                        sunLights.erase(it);
                        break;
                    }
                }
            } else {
                removedLight = lightOctreeManager->removeOctreeable(light);
            }
            onLightEvent(removedLight.get(), LightManager::REMOVE_LIGHT);
        }
        return removedLight;
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
        for(auto& sunLight : sunLights) {
            visibleLights.push_back(sunLight.get());
        }
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
        std::sort(visibleLights.begin(), visibleLights.end(), std::greater<>());
    }

    void LightManager::loadVisibleLights(GenericRenderer& lightingRenderer) {
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

        lightingRenderer.updateUniformData(2, lightsData.data());
        lightingRenderer.updateUniformData(3, &globalAmbientColor);
    }

    void LightManager::postUpdateVisibleLights() {
        lightOctreeManager->postRefreshOctreeables();
    }

    void LightManager::logMaxLightsReach() {
        static bool maxLightReachLogged = false;
        if (!maxLightReachLogged) {
            Logger::instance().logWarning("Light in scene (" + std::to_string(visibleLights.size()) + ") is higher that max light (" + std::to_string(maxLights) + ") authorized");
            maxLightReachLogged = true;
        }
    }

    void LightManager::drawLightOctree(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) {
        debugLightOctree = OctreeRenderer::createOctreeModel(*lightOctreeManager, renderTarget, projectionMatrix);
        debugLightOctree->prepareRendering(viewMatrix);
    }

}
