#include <stdexcept>

#include "SceneLight.h"
#include <resources/light/LightReaderWriter.h>

namespace urchin {

    SceneLight::SceneLight() :
            lightManager(nullptr) {

    }

    SceneLight::~SceneLight() {
        lightManager->removeLight(light.get());
    }

    void SceneLight::setup(LightManager& lightManager) {
        if (this->lightManager) {
            throw std::invalid_argument("Cannot add the scene light on two different light managers.");
        }

        this->lightManager = &lightManager;
        lightManager.addLight(light);
    }

    void SceneLight::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setLight(LightReaderWriter::loadFrom(chunk, udaParser));
    }

    void SceneLight::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        LightReaderWriter::writeOn(chunk, *light, udaWriter);
    }

    std::string SceneLight::getName() const {
        return name;
    }

    void SceneLight::setName(const std::string& name) {
        this->name = name;
    }

    Light* SceneLight::getLight() const {
        return light.get();
    }

    void SceneLight::setLight(const std::shared_ptr<Light>& light) {
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
