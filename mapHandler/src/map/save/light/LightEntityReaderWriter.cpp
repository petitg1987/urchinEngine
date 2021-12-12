#include <map/save/light/LightEntityReaderWriter.h>

namespace urchin {

    std::unique_ptr<LightEntity> LightEntityReaderWriter::load(const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        auto lightEntity = std::make_unique<LightEntity>();

        lightEntity->setName(lightEntityChunk->getAttributeValue(NAME_ATTR));

        std::unique_ptr<Light> light = buildLight(lightEntityChunk, udaParser);
        loadProperties(*light, lightEntityChunk, udaParser);
        loadFlags(*light, lightEntityChunk, udaParser);
        lightEntity->setLight(std::move(light));

        return lightEntity;
    }

    void LightEntityReaderWriter::write(UdaChunk& lightEntityChunk, const LightEntity& lightEntity, UdaWriter& udaWriter) {
        lightEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, lightEntity.getName()));

        writeLightChunk(lightEntityChunk, *lightEntity.getLight(), udaWriter);
        writeProperties(lightEntityChunk, *lightEntity.getLight(), udaWriter);
        writeFlags(lightEntityChunk, *lightEntity.getLight(), udaWriter);
    }

    std::unique_ptr<Light> LightEntityReaderWriter::buildLight(const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        std::string lightType = lightEntityChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), lightEntityChunk);
            auto omnidirectional = std::make_unique<OmnidirectionalLight>(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = udaParser.getUniqueChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightEntityChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SUN_VALUE) {
            auto directionChunk = udaParser.getUniqueChunk(true, DIRECTION_TAG, UdaAttribute(), lightEntityChunk);

            return std::make_unique<SunLight>(directionChunk->getVector3Value());
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightEntityReaderWriter::writeLightChunk(UdaChunk& lightEntityChunk, const Light& light, UdaWriter& udaWriter) {
        if (light.getLightType() == Light::OMNIDIRECTIONAL) {
            const auto& omnidirectionalLight = static_cast<const OmnidirectionalLight&>(light);
            lightEntityChunk.addAttribute(UdaAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &lightEntityChunk);
            positionChunk.setPoint3Value(omnidirectionalLight.getPosition());

            auto& exponentialAttenuationChunk = udaWriter.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightEntityChunk);
            exponentialAttenuationChunk.setFloatValue(omnidirectionalLight.getExponentialAttenuation());
        } else if (light.getLightType() == Light::SUN) {
            const auto& sunLight = static_cast<const SunLight&>(light);
            lightEntityChunk.addAttribute(UdaAttribute(TYPE_ATTR, SUN_VALUE));

            auto& directionChunk = udaWriter.createChunk(DIRECTION_TAG, UdaAttribute(), &lightEntityChunk);
            directionChunk.setVector3Value(sunLight.getDirections()[0]);
        } else {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string(light.getLightType()));
        }
    }

    void LightEntityReaderWriter::loadProperties(Light& light, const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        auto ambientColorChunk = udaParser.getUniqueChunk(true, AMBIENT_COLOR_TAG, UdaAttribute(), lightEntityChunk);
        light.setAmbientColor(ambientColorChunk->getPoint3Value());
    }

    void LightEntityReaderWriter::writeProperties(UdaChunk& lightEntityChunk, const Light& light, UdaWriter& udaWriter) {
        auto& ambientColorChunk = udaWriter.createChunk(AMBIENT_COLOR_TAG, UdaAttribute(), &lightEntityChunk);
        ambientColorChunk.setPoint3Value(light.getAmbientColor());
    }

    void LightEntityReaderWriter::loadFlags(Light& light, const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        auto produceShadowChunk = udaParser.getUniqueChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightEntityChunk);
        light.setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightEntityReaderWriter::writeFlags(UdaChunk& lightEntityChunk, const Light& light, UdaWriter& udaWriter) {
        auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightEntityChunk);
        produceShadowChunk.setBoolValue(light.isProduceShadow());
    }

}
