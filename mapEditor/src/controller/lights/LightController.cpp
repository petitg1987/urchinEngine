#include <stdexcept>

#include "LightController.h"

namespace urchin {

    LightController::LightController() :
            AbstractController() {

    }

    std::list<const SceneLight*> LightController::getSceneLights() const {
        const std::list<SceneLight*>& sceneLights = getMapHandler()->getMap()->getSceneLights();
        std::list<const SceneLight*> constSceneLights;
        constSceneLights.insert(constSceneLights.begin(), sceneLights.begin(), sceneLights.end());

        return constSceneLights;
    }

    void LightController::addSceneLight(SceneLight* sceneLight) {
        getMapHandler()->getMap()->addSceneLight(sceneLight);

        markModified();
    }

    void LightController::removeSceneLight(const SceneLight* constSceneLight) {
        SceneLight* sceneLight = findSceneLight(constSceneLight);
        getMapHandler()->getMap()->removeSceneLight(sceneLight);

        markModified();
    }

    const SceneLight* LightController::updateSceneLightGeneralProperties(const SceneLight* constSceneLight,
                                                                         const Point3<float>& ambientColor, bool isProduceShadow) {
        SceneLight* sceneLight = findSceneLight(constSceneLight);
        Light* light = sceneLight->getLight();

        light->setAmbientColor(ambientColor);
        if (light->isProduceShadow() != isProduceShadow) {
            light->setProduceShadow(isProduceShadow);
        }

        markModified();
        return sceneLight;
    }

    const SceneLight* LightController::updateSceneOmnidirectionalLightProperties(const SceneLight* constSceneLight,
                                                                                 float attenuation, const Point3<float>& position) {
        SceneLight* sceneLight = findSceneLight(constSceneLight);
        auto* light = dynamic_cast<OmnidirectionalLight*>(sceneLight->getLight());

        light->setAttenuation(attenuation);
        light->setPosition(position);

        markModified();
        return sceneLight;
    }

    const SceneLight* LightController::updateSceneSunLightProperties(const SceneLight* constSceneLight, const Vector3<float>& direction) {
        SceneLight* sceneLight = findSceneLight(constSceneLight);
        auto* light = dynamic_cast<SunLight*>(sceneLight->getLight());

        light->setDirection(direction);

        markModified();
        return sceneLight;
    }

    SceneLight* LightController::findSceneLight(const SceneLight* constSceneLight) {
        const std::list<SceneLight*>& sceneLights = getMapHandler()->getMap()->getSceneLights();
        auto it = std::find(sceneLights.begin(), sceneLights.end(), constSceneLight);

        if (it != sceneLights.end()) {
            return *it;
        }

        throw std::invalid_argument("Impossible to find scene light: " + constSceneLight->getName());
    }

}
