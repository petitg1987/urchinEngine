#include <stdexcept>

#include "SceneLight.h"
#include <resources/light/LightReaderWriter.h>

namespace urchin {

    SceneLight::SceneLight() :
            lightManager(nullptr),
            light(nullptr) {

    }

    SceneLight::~SceneLight() {
        lightManager->removeLight(light);
    }

    void SceneLight::setLightManager(LightManager& lightManager) {
        if (this->lightManager) {
            throw std::invalid_argument("Cannot add the scene light on two different light managers.");
        }

        this->lightManager = &lightManager;
        lightManager.addLight(light);
    }

    void SceneLight::loadFrom(const DataChunk* chunk, const DataParser& dataParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setLight(LightReaderWriter::loadFrom(chunk, dataParser));
    }

    void SceneLight::writeOn(DataChunk* chunk, DataWriter& dataWriter) const {
        chunk->setAttribute(DataAttribute(NAME_ATTR, this->name));

        LightReaderWriter::writeOn(chunk, light, dataWriter);
    }

    std::string SceneLight::getName() const {
        return name;
    }

    void SceneLight::setName(const std::string& name) {
        this->name = name;
    }

    Light* SceneLight::getLight() const {
        return light;
    }

    void SceneLight::setLight(Light* light) {
        if (!light) {
            throw std::invalid_argument("Cannot set a null light on scene light.");
        }

        if (lightManager) {
            lightManager->removeLight(this->light);
            lightManager->addLight(light);
        } else {
            delete this->light;
        }

        this->light = light;
    }

}
