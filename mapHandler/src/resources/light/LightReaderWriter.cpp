#include <resources/light/LightReaderWriter.h>

namespace urchin {

    std::shared_ptr<Light> LightReaderWriter::loadFrom(const UdaChunk* lightChunk, const UdaParser& udaParser) {
        std::shared_ptr<Light> light = buildLightFrom(lightChunk, udaParser);

        loadPropertiesFrom(*light, lightChunk, udaParser);
        loadFlagsFrom(*light, lightChunk, udaParser);

        return light;
    }

    void LightReaderWriter::writeOn(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        buildChunkFrom(lightChunk, light, udaWriter);

        writePropertiesOn(lightChunk, light, udaWriter);
        writeFlagsOn(lightChunk, light, udaWriter);
    }

    std::shared_ptr<Light> LightReaderWriter::buildLightFrom(const UdaChunk* lightChunk, const UdaParser& udaParser) {
        std::string lightType = lightChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), lightChunk);
            auto omnidirectional = std::make_shared<OmnidirectionalLight>(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = udaParser.getUniqueChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SUN_VALUE) {
            auto directionChunk = udaParser.getUniqueChunk(true, DIRECTION_TAG, UdaAttribute(), lightChunk);

            return std::make_shared<SunLight>(directionChunk->getVector3Value());
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightReaderWriter::buildChunkFrom(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        if (light.getLightType() == Light::OMNIDIRECTIONAL) {
            const auto& omnidirectionalLight = dynamic_cast<const OmnidirectionalLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &lightChunk);
            positionChunk.setPoint3Value(omnidirectionalLight.getPosition());

            auto& exponentialAttenuationChunk = udaWriter.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightChunk);
            exponentialAttenuationChunk.setFloatValue(omnidirectionalLight.getExponentialAttenuation());
        } else if (light.getLightType() == Light::SUN) {
            const auto& sunLight = dynamic_cast<const SunLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, SUN_VALUE));

            auto& directionChunk = udaWriter.createChunk(DIRECTION_TAG, UdaAttribute(), &lightChunk);
            directionChunk.setVector3Value(sunLight.getDirections()[0]);
        } else {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string(light.getLightType()));
        }
    }

    void LightReaderWriter::loadPropertiesFrom(Light& light, const UdaChunk* lightChunk, const UdaParser& udaParser) {
        auto ambientColorChunk = udaParser.getUniqueChunk(true, AMBIENT_COLOR_TAG, UdaAttribute(), lightChunk);
        light.setAmbientColor(ambientColorChunk->getPoint3Value());
    }

    void LightReaderWriter::writePropertiesOn(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        auto& ambientColorChunk = udaWriter.createChunk(AMBIENT_COLOR_TAG, UdaAttribute(), &lightChunk);
        ambientColorChunk.setPoint3Value(light.getAmbientColor());
    }

    void LightReaderWriter::loadFlagsFrom(Light& light, const UdaChunk* lightChunk, const UdaParser& udaParser) {
        auto produceShadowChunk = udaParser.getUniqueChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightChunk);
        light.setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightReaderWriter::writeFlagsOn(UdaChunk& lightChunk, const Light& light, UdaWriter& udaWriter) {
        auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightChunk);
        produceShadowChunk.setBoolValue(light.isProduceShadow());
    }

}
