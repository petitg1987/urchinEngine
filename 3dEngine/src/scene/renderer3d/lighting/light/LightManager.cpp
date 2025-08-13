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
        std::ranges::transform(sunLights, std::back_inserter(visibleLights),[](const std::shared_ptr<Light>& sunLight) { return sunLight.get(); });
        visibleLights.insert(visibleLights.end(), lightsInFrustum.begin(), lightsInFrustum.end());
        visibleLights.resize(std::min((std::size_t)MAX_LIGHTS, visibleLights.size()));
    }

    void LightManager::loadVisibleLights(GenericRenderer& deferredSecondPassRenderer, uint32_t lightsDataBinding) {
        std::span<Light* const> lights = getVisibleLights();

        lightsData.lightsCount = (unsigned int)lights.size();

        std::size_t lightIndex = 0;
        for (Light* const light : lights) {
            LightInfo& lightInfo = lightsData.lightsInfo[lightIndex++];

            lightInfo.lightType = (int)light->getLightType();
            lightInfo.lightFlags =
                    (light->isProduceShadow() ? Light::LIGHT_FLAG_PRODUCE_SHADOW : 0) |
                    (light->isPbrEnabled() ? Light::LIGHT_FLAG_PBR_ENABLED : 0);
            lightInfo.lightMask = light->getLightMask();
            lightInfo.shadowStrength = light->getShadowStrength();
            lightInfo.lightColor = light->getLightColor();

            if (light->getLightType() == Light::LightType::SUN) {
                const auto* sunLight = dynamic_cast<const SunLight*>(light);
                lightInfo.direction = sunLight->getDirections()[0];
            } else if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light);
                lightInfo.position = omnidirectionalLight->getPosition();
                lightInfo.exponentialAttenuation = omnidirectionalLight->getExponentialAttenuation();
            } else if (light->getLightType() == Light::LightType::SPOT) {
                const auto* spotLight = dynamic_cast<const SpotLight*>(light);
                lightInfo.position = spotLight->getPosition();
                lightInfo.direction = spotLight->getDirections()[0];
                lightInfo.exponentialAttenuation = spotLight->getExponentialAttenuation();
                lightInfo.innerCosAngle = spotLight->getInnerCosAngle();
                lightInfo.outerCosAngle = spotLight->getOuterCosAngle();
            } else {
                throw std::invalid_argument("Unknown light type to load shader variables: " + std::to_string((int)light->getLightType()));
            }
        }

        std::size_t dataUpdatedSize = 16 /* globalAmbientColor + lightsCount */ + lightsData.lightsCount * sizeof(LightInfo);
        deferredSecondPassRenderer.updateStorageBufferPartialData(lightsDataBinding, dataUpdatedSize, &lightsData);
    }

    void LightManager::postUpdateVisibleLights() {
        lightOctreeManager.postRefreshOctreeables();
    }

    void LightManager::drawLightOctree(GeometryContainer& geometryContainer) {
        if (debugLightOctree) {
            geometryContainer.removeGeometry(*debugLightOctree);
        }
        debugLightOctree = OctreeRenderer::createOctreeModel(lightOctreeManager);
        geometryContainer.addGeometry(debugLightOctree);
    }

}
