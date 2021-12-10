#include <stdexcept>

#include <resources/light/LightEntity.h>
#include <resources/light/LightReaderWriter.h>

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

    void LightEntity::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setLight(LightReaderWriter::loadFrom(chunk, udaParser));
    }

    void LightEntity::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        LightReaderWriter::writeOn(chunk, *light, udaWriter);
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
