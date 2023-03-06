#include <stdexcept>

#include <scene/renderer3d/lighting/light/LightManager.h>
#include <scene/renderer3d/lighting/light/sun/SunLight.h>
#include <scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

namespace urchin {

    LightManager::LightManager() :
            lightOctreeManager(OctreeManager<Light>(ConfigService::instance().getFloatValue("light.octreeMinSize"))),
            lastUpdatedLight(nullptr),
            lightsData({}) {
        static_assert(MAX_LIGHTS < LIGHTS_SHADER_LIMIT);
        lightsData.globalAmbientColor = Point3<float>(0.0f, 0.0f, 0.0f);
        LightInfo emptyLightData{};
        for (auto& light : lightsData.lightsInfo) {
            light = emptyLightData;
        }
    }

    void LightManager::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder) const {
        lightingRendererBuilder
                ->addUniformData(sizeof(LightsData), &lightsData);
    }

    OctreeManager<Light>& LightManager::getLightOctreeManager() {
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
        return MAX_LIGHTS;
    }

    const std::vector<std::shared_ptr<Light>>& LightManager::getSunLights() const {
        return sunLights;
    }

    std::span<Light* const> LightManager::getVisibleLights() const {
        return visibleLights;
    }

    void LightManager::addLight(std::shared_ptr<Light> light) {
        if (light) {
            ScopeProfiler sp(Profiler::graphic(), "addLight");

            Light* lightPtr = light.get();
            if (light->getLightType() == Light::LightType::SUN) {
                sunLights.push_back(std::move(light));
            } else {
                lightOctreeManager.addOctreeable(std::move(light));
            }

            onLightEvent(lightPtr, LightManager::ADD_LIGHT);
        }
    }

    std::shared_ptr<Light> LightManager::removeLight(Light* light) {
        std::shared_ptr<Light> removedLight;
        if (light) {
            if (light->getLightType() == Light::LightType::SUN) {
                for (auto it = sunLights.begin(); it != sunLights.end(); ++it) {
                    if (it->get() == light) {
                        removedLight = std::move(*it);
                        sunLights.erase(it);
                        break;
                    }
                }
            } else {
                removedLight = lightOctreeManager.removeOctreeable(light);
            }
            onLightEvent(removedLight.get(), LightManager::REMOVE_LIGHT);
        }
        return removedLight;
    }

    void LightManager::setGlobalAmbientColor(const Point3<float>& globalAmbientColor) {
        this->lightsData.globalAmbientColor = globalAmbientColor;
    }

    const Point3<float>& LightManager::getGlobalAmbientColor() const {
        return lightsData.globalAmbientColor;
    }

    void LightManager::updateVisibleLights(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "updateLights");

        lightOctreeManager.refreshOctreeables();
        lightsInFrustum.clear();
        lightOctreeManager.getOctreeablesIn(frustum, lightsInFrustum);

        visibleLights.clear();
        for (const auto& sunLight : sunLights) {
            visibleLights.push_back(sunLight.get());
        }
        visibleLights.insert(visibleLights.end(), lightsInFrustum.begin(), lightsInFrustum.end());

        if (visibleLights.size() > MAX_LIGHTS) {
            logMaxLightsReach();
            visibleLights.resize(MAX_LIGHTS);
        }

        /*
         * Sort visible lights by their memory address.
         * By sorting visible lights, we make sure that the lights producing shadow are often in same order.
         * Thus, the shadow textures positions/index in shader don't need to be updated at each frame in ShadowManager#loadShadowMaps().
         */
        std::ranges::sort(visibleLights, std::greater<>());
    }

    void LightManager::loadVisibleLights(GenericRenderer& lightingRenderer, std::size_t lightsDataUniformIndex) {
        std::span<Light* const> lights = getVisibleLights();

        for (unsigned int i = 0; i < MAX_LIGHTS; ++i) {
            if (lights.size() > i) {
                const Light* light = lights[i];

                lightsData.lightsInfo[i].isExist = true;
                lightsData.lightsInfo[i].produceShadow = light->isProduceShadow();
                lightsData.lightsInfo[i].lightType = (int)light->getLightType();
                lightsData.lightsInfo[i].lightColor = light->getLightColor();

                if (lights[i]->getLightType() == Light::LightType::SUN) {
                    const auto* sunLight = static_cast<const SunLight*>(light);
                    lightsData.lightsInfo[i].direction = sunLight->getDirections()[0];
                } else if (lights[i]->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                    const auto* omnidirectionalLight = static_cast<const OmnidirectionalLight*>(light);
                    lightsData.lightsInfo[i].position = omnidirectionalLight->getPosition().toVector();
                    lightsData.lightsInfo[i].exponentialAttenuation = omnidirectionalLight->getExponentialAttenuation();
                } else {
                    throw std::invalid_argument("Unknown light type to load shader variables: " + std::to_string((int)light->getLightType()));
                }
            } else {
                lightsData.lightsInfo[i].isExist = false;
                break;
            }
        }

        lightingRenderer.updateUniformData(lightsDataUniformIndex, &lightsData);
    }

    void LightManager::postUpdateVisibleLights() {
        lightOctreeManager.postRefreshOctreeables();
    }

    void LightManager::logMaxLightsReach() const {
        static bool maxLightReachLogged = false;
        if (!maxLightReachLogged) {
            Logger::instance().logWarning("Light in scene (" + std::to_string(visibleLights.size()) + ") is higher that max light (" + std::to_string(MAX_LIGHTS) + ") authorized");
            maxLightReachLogged = true;
        }
    }

    void LightManager::drawLightOctree(GeometryContainer& geometryContainer) {
        if (debugLightOctree) {
            geometryContainer.removeGeometry(*debugLightOctree);
        }
        debugLightOctree = OctreeRenderer::createOctreeModel(lightOctreeManager);
        geometryContainer.addGeometry(debugLightOctree);
    }

}
