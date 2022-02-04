#include <stdexcept>

#include <model/LightEntity.h>

namespace urchin {

    LightEntity::LightEntity() :
            lightManager(nullptr) {

    }

    LightEntity::~LightEntity() {
        if (lightManager) {
            lightManager->removeLight(light.get());
        }
    }

    void LightEntity::setup(LightManager& lightManager) {
        this->lightManager = &lightManager;
        lightManager.addLight(light);
    }

    std::string LightEntity::getName() const {
        return name;
    }

    void LightEntity::setName(std::string name) {
        this->name = std::move(name);
    }

    Light* LightEntity::getLight() const {
        return light.get();
    }

    void LightEntity::setLight(const std::shared_ptr<Light>& light) {
        if (lightManager) {
            lightManager->removeLight(this->light.get());
            lightManager->addLight(light);
        }

        this->light = light;
    }

}
