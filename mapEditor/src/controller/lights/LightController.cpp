#include <stdexcept>

#include <controller/lights/LightController.h>

namespace urchin {

    LightController::LightController() :
            AbstractController() {

    }

    std::list<const SceneLight*> LightController::getSceneLights() const {
        const auto& sceneLights = getMapHandler()->getMap().getSceneLights();
        std::list<const SceneLight*> constSceneLights;
        for (auto& sceneLight : sceneLights) {
            constSceneLights.emplace_back(sceneLight.get());
        }

        return constSceneLights;
    }

    void LightController::addSceneLight(std::unique_ptr<SceneLight> sceneLight) {
        getMapHandler()->getMap().addSceneLight(std::move(sceneLight));

        markModified();
    }

    void LightController::removeSceneLight(const SceneLight& constSceneLight) {
        SceneLight& sceneLight = findSceneLight(constSceneLight);
        getMapHandler()->getMap().removeSceneLight(sceneLight);

        markModified();
    }

    const SceneLight& LightController::updateSceneLightGeneralProperties(const SceneLight& constSceneLight,
                                                                         const Point3<float>& ambientColor, bool isProduceShadow) {
        const SceneLight& sceneLight = findSceneLight(constSceneLight);
        Light* light = sceneLight.getLight();

        light->setAmbientColor(ambientColor);
        if (light->isProduceShadow() != isProduceShadow) {
            light->setProduceShadow(isProduceShadow);
        }

        markModified();
        return sceneLight;
    }

    const SceneLight& LightController::updateSceneOmnidirectionalLightProperties(const SceneLight& constSceneLight,
                                                                                 float attenuation, const Point3<float>& position) {
        const SceneLight& sceneLight = findSceneLight(constSceneLight);
        auto* light = dynamic_cast<OmnidirectionalLight*>(sceneLight.getLight());

        light->setAttenuation(attenuation);
        light->setPosition(position);

        markModified();
        return sceneLight;
    }

    const SceneLight& LightController::updateSceneSunLightProperties(const SceneLight& constSceneLight, const Vector3<float>& direction) {
        const SceneLight& sceneLight = findSceneLight(constSceneLight);
        auto* light = dynamic_cast<SunLight*>(sceneLight.getLight());

        light->setDirection(direction);

        markModified();
        return sceneLight;
    }

    SceneLight& LightController::findSceneLight(const SceneLight& constSceneLight) {
        const auto& sceneLights = getMapHandler()->getMap().getSceneLights();
        auto it = std::find_if(sceneLights.begin(), sceneLights.end(), [&constSceneLight](const auto& o){return o.get() == &constSceneLight;});

        if (it != sceneLights.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find scene light: " + constSceneLight.getName());
    }

}
