#include <resources/light/LightEntityReaderWriter.h>

namespace urchin {

    std::unique_ptr<LightEntity> LightEntityReaderWriter::loadFrom(const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        auto lightEntity = std::make_unique<LightEntity>();

        lightEntity->setName(lightEntityChunk->getAttributeValue(NAME_ATTR));

        std::unique_ptr<Light> light = buildLightFrom(lightEntityChunk, udaParser);
        loadPropertiesFrom(*light, lightEntityChunk, udaParser);
        loadFlagsFrom(*light, lightEntityChunk, udaParser);
        lightEntity->setLight(std::move(light));

        return lightEntity;
    }

    void LightEntityReaderWriter::writeOn(UdaChunk& chunk, const LightEntity& lightEntity, UdaWriter& udaWriter) {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, lightEntity.getName()));

        buildChunkFrom(chunk, *lightEntity.getLight(), udaWriter);
        writePropertiesOn(chunk, *lightEntity.getLight(), udaWriter);
        writeFlagsOn(chunk, *lightEntity.getLight(), udaWriter);
    }

    std::unique_ptr<Light> LightEntityReaderWriter::buildLightFrom(const UdaChunk* lightChunk, const UdaParser& udaParser) {
        std::string lightType = lightChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), lightChunk);
            auto omnidirectional = std::make_unique<OmnidirectionalLight>(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = udaParser.getUniqueChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SUN_VALUE) {
            auto directionChunk = udaParser.getUniqueChunk(true, DIRECTION_TAG, UdaAttribute(), lightChunk);

            return std::make_unique<SunLight>(directionChunk->getVector3Value());
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightEntityReaderWriter::buildChunkFrom(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        if (light.getLightType() == Light::OMNIDIRECTIONAL) {
            const auto& omnidirectionalLight = static_cast<const OmnidirectionalLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &lightChunk);
            positionChunk.setPoint3Value(omnidirectionalLight.getPosition());

            auto& exponentialAttenuationChunk = udaWriter.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightChunk);
            exponentialAttenuationChunk.setFloatValue(omnidirectionalLight.getExponentialAttenuation());
        } else if (light.getLightType() == Light::SUN) {
            const auto& sunLight = static_cast<const SunLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, SUN_VALUE));

            auto& directionChunk = udaWriter.createChunk(DIRECTION_TAG, UdaAttribute(), &lightChunk);
            directionChunk.setVector3Value(sunLight.getDirections()[0]);
        } else {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string(light.getLightType()));
        }
    }

    void LightEntityReaderWriter::loadPropertiesFrom(Light& light, const UdaChunk* lightChunk, const UdaParser& udaParser) {
        auto ambientColorChunk = udaParser.getUniqueChunk(true, AMBIENT_COLOR_TAG, UdaAttribute(), lightChunk);
        light.setAmbientColor(ambientColorChunk->getPoint3Value());
    }

    void LightEntityReaderWriter::writePropertiesOn(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        auto& ambientColorChunk = udaWriter.createChunk(AMBIENT_COLOR_TAG, UdaAttribute(), &lightChunk);
        ambientColorChunk.setPoint3Value(light.getAmbientColor());
    }

    void LightEntityReaderWriter::loadFlagsFrom(Light& light, const UdaChunk* lightChunk, const UdaParser& udaParser) {
        auto produceShadowChunk = udaParser.getUniqueChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightChunk);
        light.setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightEntityReaderWriter::writeFlagsOn(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightChunk);
        produceShadowChunk.setBoolValue(light.isProduceShadow());
    }

}
