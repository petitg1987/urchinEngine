#include <stdexcept>

#include <resources/light/LightEntity.h>

namespace urchin {

    LightEntity::LightEntity() :
            lightManager(nullptr) {

    }

    LightEntity::~LightEntity() {
        lightManager->removeLight(light.get());
    }

    void LightEntity::setup(LightManager& lightManager) {
        if (this->lightManager) {
            throw std::invalid_argument("Cannot add the scene light on two different light managers.");
        }

        this->lightManager = &lightManager;
        lightManager.addLight(light);
    }

    std::string LightEntity::getName() const {
        return name;
    }

    void LightEntity::setName(const std::string& name) {
        this->name = name;
    }

    Light* LightEntity::getLight() const {
        return light.get();
    }

    void LightEntity::setLight(const std::shared_ptr<Light>& light) {
        if (!light) {
            throw std::invalid_argument("Cannot set a null light on scene light.");
        }

        if (lightManager) {
            lightManager->removeLight(this->light.get());
            lightManager->addLight(light);
        }

        this->light = light;
    }

}
