#include <stdexcept>

#include <controller/lights/LightController.h>

namespace urchin {

    LightController::LightController() :
            AbstractController() {

    }

    std::list<const LightEntity*> LightController::getLightEntities() const {
        const auto& lightEntities = getMapHandler()->getMap().getLightEntities();
        std::list<const LightEntity*> constLightEntities;
        for (auto& lightEntity : lightEntities) {
            constLightEntities.emplace_back(lightEntity.get());
        }

        return constLightEntities;
    }

    void LightController::addLightEntity(std::unique_ptr<LightEntity> lightEntity) {
        getMapHandler()->getMap().addLightEntity(std::move(lightEntity));

        markModified();
    }

    void LightController::removeLightEntity(const LightEntity& constLightEntity) {
        LightEntity& lightEntity = findLightEntity(constLightEntity);
        getMapHandler()->getMap().removeLightEntity(lightEntity);

        markModified();
    }

    const LightEntity& LightController::updateLightGeneralProperties(const LightEntity& constLightEntity,
                                                                     const Point3<float>& ambientColor, bool isProduceShadow) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        Light* light = lightEntity.getLight();

        light->setAmbientColor(ambientColor);
        if (light->isProduceShadow() != isProduceShadow) {
            light->setProduceShadow(isProduceShadow);
        }

        markModified();
        return lightEntity;
    }

    const LightEntity& LightController::updateOmnidirectionalLightProperties(const LightEntity& constLightEntity,
                                                                             float attenuation, const Point3<float>& position) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        auto* light = static_cast<OmnidirectionalLight*>(lightEntity.getLight());

        light->setAttenuation(attenuation);
        light->setPosition(position);

        markModified();
        return lightEntity;
    }

    const LightEntity& LightController::updateSunLightProperties(const LightEntity& constLightEntity, const Vector3<float>& direction) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        auto* light = static_cast<SunLight*>(lightEntity.getLight());

        light->setDirection(direction);

        markModified();
        return lightEntity;
    }

    LightEntity& LightController::findLightEntity(const LightEntity& constLightEntity) {
        const auto& lightEntities = getMapHandler()->getMap().getLightEntities();
        auto it = std::ranges::find_if(lightEntities, [&constLightEntity](const auto& o){return o.get() == &constLightEntity;});

        if (it != lightEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find light entity: " + constLightEntity.getName());
    }

}
