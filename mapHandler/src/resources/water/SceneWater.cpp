#include <stdexcept>

#include "SceneWater.h"
#include <resources/water/WaterReaderWriter.h>

namespace urchin {
    SceneWater::SceneWater() :
            renderer3d(nullptr),
            water(nullptr) {

    }

    SceneWater::~SceneWater() {
        renderer3d->getWaterManager().removeWater(water);
    }

    void SceneWater::setWaterManagers(Renderer3d* renderer3d) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the scene water on two different renderer.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer manager for a scene water.");
        }

        this->renderer3d = renderer3d;

        renderer3d->getWaterManager().addWater(water);
    }

    void SceneWater::loadFrom(const UdaChunk* chunk, const DataParser& dataParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setWater(WaterReaderWriter().loadFrom(chunk, dataParser));
    }

    void SceneWater::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        WaterReaderWriter().writeOn(chunk, water, udaWriter);
    }

    std::string SceneWater::getName() const {
        return name;
    }

    void SceneWater::setName(const std::string& name) {
        this->name = name;
    }

    Water* SceneWater::getWater() const {
        return water;
    }

    void SceneWater::setWater(Water* water) {
        if (!water) {
            throw std::invalid_argument("Cannot set a null water on scene water.");
        }

        if (renderer3d) {
            renderer3d->getWaterManager().removeWater(this->water);
            renderer3d->getWaterManager().addWater(water);
        } else {
            delete this->water;
        }

        this->water = water;
    }

}
