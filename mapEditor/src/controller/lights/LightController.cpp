#include <stdexcept>

#include <controller/lights/LightController.h>
#include <controller/EntityControllerUtil.h>

namespace urchin {

    std::list<const LightEntity*> LightController::getLightEntities() const {
        const auto& lightEntities = getMap().getLightEntities();
        std::list<const LightEntity*> constLightEntities;
        for (auto& lightEntity : lightEntities) {
            constLightEntities.emplace_back(lightEntity.get());
        }

        return constLightEntities;
    }

    void LightController::addLightEntity(std::unique_ptr<LightEntity> lightEntity) {
        getMap().addLightEntity(std::move(lightEntity));

        markModified();
    }

    void LightController::moveLightInFrontOfCamera(const LightEntity& constLightEntity, bool isClonedEntity) { //TODO use this method somewhere before removing the class
        if (constLightEntity.getLight()->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
            auto* omnidirectionalLight = static_cast<OmnidirectionalLight*>(constLightEntity.getLight());
            Point3<float> currentPosition = omnidirectionalLight->getPosition();
            Point3<float> newPosition = EntityControllerUtil::determineNewPosition(currentPosition, isClonedEntity, getMap().getRenderer3d()->getCamera());
            omnidirectionalLight->setPosition(newPosition);

            markModified();
        }
    }

    void LightController::removeLightEntity(const LightEntity& constLightEntity) {
        LightEntity& lightEntity = findLightEntity(constLightEntity);
        getMap().removeLightEntity(lightEntity);

        markModified();
    }

    const LightEntity& LightController::updateLightGeneralProperties(const LightEntity& constLightEntity, const Point3<float>& lightColor, bool enablePbr, bool isProduceShadow) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        Light* light = lightEntity.getLight();

        light->setLightColor(lightColor);
        light->setPbrEnabled(enablePbr);
        if (light->isProduceShadow() != isProduceShadow) {
            light->setProduceShadow(isProduceShadow);
        }

        markModified();
        return lightEntity;
    }

    const LightEntity& LightController::updateSunLightProperties(const LightEntity& constLightEntity, const Vector3<float>& direction) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        auto* sunLight = static_cast<SunLight*>(lightEntity.getLight());

        sunLight->setDirection(direction);

        markModified();
        return lightEntity;
    }

    const LightEntity& LightController::updateOmnidirectionalLightProperties(const LightEntity& constLightEntity, float attenuation, const Point3<float>& position) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        auto* omnidirectionalLight = static_cast<OmnidirectionalLight*>(lightEntity.getLight());

        omnidirectionalLight->setAttenuation(attenuation);
        omnidirectionalLight->setPosition(position);

        markModified();
        return lightEntity;
    }

    const LightEntity& LightController::updateSpotLightProperties(const LightEntity& constLightEntity, float attenuation, const Point3<float>& position, const Vector3<float>& direction,
                                                                  float innerAngleInDegrees, float outerAngleInDegrees) {
        const LightEntity& lightEntity = findLightEntity(constLightEntity);
        auto* spotLight = static_cast<SpotLight*>(lightEntity.getLight());

        spotLight->setAttenuation(attenuation);
        spotLight->setPosition(position);
        spotLight->setDirection(direction);
        spotLight->setAngles(innerAngleInDegrees, outerAngleInDegrees);

        markModified();
        return lightEntity;
    }

    LightEntity& LightController::findLightEntity(const LightEntity& constLightEntity) const {
        const auto& lightEntities = getMap().getLightEntities();
        auto it = std::ranges::find_if(lightEntities, [&constLightEntity](const auto& o){return o.get() == &constLightEntity;});

        if (it != lightEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find light entity: " + constLightEntity.getName());
    }

}
