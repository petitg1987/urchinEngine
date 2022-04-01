#include <stdexcept>

#include <model/LightEntity.h>

namespace urchin {

    LightEntity::LightEntity() :
            renderer3d(nullptr) {

    }

    LightEntity::~LightEntity() {
        if (renderer3d) {
            renderer3d->getLightManager().removeLight(light.get());
        }
    }

    void LightEntity::setup(Renderer3d* renderer3d) {
        this->renderer3d = renderer3d;
        if (renderer3d) {
            renderer3d->getLightManager().addLight(light);
        }
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
        if (renderer3d) {
            renderer3d->getLightManager().removeLight(this->light.get());
            renderer3d->getLightManager().addLight(light);
        }

        this->light = light;
    }

}
