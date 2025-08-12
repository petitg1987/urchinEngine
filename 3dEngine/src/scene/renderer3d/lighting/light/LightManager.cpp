#include <stdexcept>
#include <cstring>

#include "scene/renderer3d/lighting/light/LightManager.h"
#include "scene/renderer3d/lighting/light/sun/SunLight.h"
#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/lighting/light/spot/SpotLight.h"
#include "scene/renderer3d/util/OctreeRenderer.h"

namespace urchin {

    LightManager::LightManager() :
            lightOctreeManager(OctreeManager<Light>(ConfigService::instance().getFloatValue("light.octreeMinSize"))),
            lastUpdatedLight(nullptr),
            lightsData({}) {
        static_assert(MAX_LIGHTS < LIGHTS_SHADER_LIMIT);
        std::memset((void*)&lightsData, 0, sizeof(lightsData));

        lightsData.globalAmbientColor = Point3(0.0f, 0.0f, 0.0f);
        LightInfo emptyLightData{};
        for (auto& light : lightsData.lightsInfo) {
            light = emptyLightData;
        }
    }

    void LightManager::setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>& deferredSecondPassRendererBuilder, uint32_t lightsDataBinding) const {
        deferredSecondPassRendererBuilder
                ->addStorageBufferData(lightsDataBinding, sizeof(LightsData), &lightsData);
    }

    OctreeManager<Light>& LightManager::getLightOctreeManager() {
        return lightOctreeManager;
    }

    void LightManager::onLightEvent(Light* light, NotificationType notificationType) {
        lastUpdatedLight = light;
        notifyObservers(this, notificationType);
    }

    Light* LightManager::getLastUpdatedLight() const {
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

            onLightEvent(lightPtr, ADD_LIGHT);
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
            onLightEvent(removedLight.get(), REMOVE_LIGHT);
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
        lightOctreeManager.getOctreeablesIn(frustum, lightsInFrustum, false);

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
        std::ranges::sort(visibleLights, std::greater());
    }

    void LightManager::loadVisibleLights(GenericRenderer& deferredSecondPassRenderer, uint32_t lightsDataBinding) {
        std::span<Light* const> lights = getVisibleLights();

        for (unsigned int i = 0; i < MAX_LIGHTS; ++i) {
            LightInfo& currentLightInfo = lightsData.lightsInfo[i];

            if (lights.size() > i) {
                const Light* light = lights[i];

                currentLightInfo.isExist = true;
                currentLightInfo.lightType = (int)light->getLightType();
                currentLightInfo.lightFlags =
                        (light->isProduceShadow() ? Light::LIGHT_FLAG_PRODUCE_SHADOW : 0) |
                        (light->isPbrEnabled() ? Light::LIGHT_FLAG_PBR_ENABLED : 0);
                currentLightInfo.lightMask = light->getLightMask();
                currentLightInfo.shadowStrength = light->getShadowStrength();
                currentLightInfo.lightColor = light->getLightColor();

                if (lights[i]->getLightType() == Light::LightType::SUN) {
                    const auto* sunLight = dynamic_cast<const SunLight*>(light);
                    currentLightInfo.direction = sunLight->getDirections()[0];
                } else if (lights[i]->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                    const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light);
                    currentLightInfo.position = omnidirectionalLight->getPosition();
                    currentLightInfo.exponentialAttenuation = omnidirectionalLight->getExponentialAttenuation();
                } else if (lights[i]->getLightType() == Light::LightType::SPOT) {
                    const auto* spotLight = dynamic_cast<const SpotLight*>(light);
                    currentLightInfo.position = spotLight->getPosition();
                    currentLightInfo.direction = spotLight->getDirections()[0];
                    currentLightInfo.exponentialAttenuation = spotLight->getExponentialAttenuation();
                    currentLightInfo.innerCosAngle = spotLight->getInnerCosAngle();
                    currentLightInfo.outerCosAngle = spotLight->getOuterCosAngle();
                } else {
                    throw std::invalid_argument("Unknown light type to load shader variables: " + std::to_string((int)light->getLightType()));
                }
            } else {
                currentLightInfo.isExist = false;
                break;
            }
        }

        deferredSecondPassRenderer.updateStorageBufferData(lightsDataBinding, &lightsData);
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
