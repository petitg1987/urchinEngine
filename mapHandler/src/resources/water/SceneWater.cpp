#include <stdexcept>

#include "SceneWater.h"
#include <resources/water/WaterReaderWriter.h>

namespace urchin {
    SceneWater::SceneWater() :
            renderer3d(nullptr),
            water(nullptr) {

    }

    SceneWater::~SceneWater() {
        if (water) {
            renderer3d->getWaterContainer().removeWater(*water);
        }
    }

    void SceneWater::setup(Renderer3d* renderer3d) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the scene water on two different renderer.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer manager for a scene water.");
        }

        this->renderer3d = renderer3d;

        renderer3d->getWaterContainer().addWater(water);
    }

    void SceneWater::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setWater(WaterReaderWriter().loadFrom(chunk, udaParser));
    }

    void SceneWater::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        WaterReaderWriter().writeOn(chunk, *water, udaWriter);
    }

    std::string SceneWater::getName() const {
        return name;
    }

    void SceneWater::setName(const std::string& name) {
        this->name = name;
    }

    Water* SceneWater::getWater() const {
        return water.get();
    }

    void SceneWater::setWater(std::shared_ptr<Water> water) {
        if (!water) {
            throw std::invalid_argument("Cannot set a null water on scene water.");
        }

        if (renderer3d) {
            if (this->water) {
                renderer3d->getWaterContainer().removeWater(*this->water);
            }
            renderer3d->getWaterContainer().addWater(water);
        }

        this->water = std::move(water);
    }

}
