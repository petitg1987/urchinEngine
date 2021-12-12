#include <stdexcept>

#include <resources/water/WaterEntity.h>

namespace urchin {
    WaterEntity::WaterEntity() :
            renderer3d(nullptr),
            water(nullptr) {

    }

    WaterEntity::~WaterEntity() {
        if (water) {
            renderer3d->getWaterContainer().removeWater(*water);
        }
    }

    void WaterEntity::setup(Renderer3d& renderer3d) {
        this->renderer3d = &renderer3d;

        renderer3d.getWaterContainer().addWater(water);
    }

    std::string WaterEntity::getName() const {
        return name;
    }

    void WaterEntity::setName(const std::string& name) {
        this->name = name;
    }

    Water* WaterEntity::getWater() const {
        return water.get();
    }

    void WaterEntity::setWater(std::shared_ptr<Water> water) {
        if (renderer3d) {
            if (this->water) {
                renderer3d->getWaterContainer().removeWater(*this->water);
            }
            renderer3d->getWaterContainer().addWater(water);
        }

        this->water = std::move(water);
    }

}
