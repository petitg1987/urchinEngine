#include "LightReaderWriter.h"

namespace urchin {

    Light* LightReaderWriter::loadFrom(const DataChunk* lightChunk, const DataParser& dataParser) {
        Light* light = buildLightFrom(lightChunk, dataParser);

        loadPropertiesFrom(light, lightChunk, dataParser);
        loadFlagsFrom(light, lightChunk, dataParser);

        return light;
    }

    void LightReaderWriter::writeOn(DataChunk& lightChunk, const Light* light, UdaWriter& udaWriter) {
        buildChunkFrom(lightChunk, light, udaWriter);

        writePropertiesOn(lightChunk, light, udaWriter);
        writeFlagsOn(lightChunk, light, udaWriter);
    }

    Light* LightReaderWriter::buildLightFrom(const DataChunk* lightChunk, const DataParser& dataParser) {
        std::string lightType = lightChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), lightChunk);
            auto* omnidirectional = new OmnidirectionalLight(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = dataParser.getUniqueChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SUN_VALUE) {
            auto directionChunk = dataParser.getUniqueChunk(true, DIRECTION_TAG, UdaAttribute(), lightChunk);

            return new SunLight(directionChunk->getVector3Value());
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightReaderWriter::buildChunkFrom(DataChunk& lightChunk, const Light* light, UdaWriter& udaWriter) {
        if (light->getLightType() == Light::OMNIDIRECTIONAL) {
            const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &lightChunk);
            positionChunk.setPoint3Value(omnidirectionalLight->getPosition());

            auto& exponentialAttenuationChunk = udaWriter.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightChunk);
            exponentialAttenuationChunk.setFloatValue(omnidirectionalLight->getExponentialAttenuation());
        } else if (light->getLightType() == Light::SUN) {
            const auto* sunLight = dynamic_cast<const SunLight*>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, SUN_VALUE));

            auto& directionChunk = udaWriter.createChunk(DIRECTION_TAG, UdaAttribute(), &lightChunk);
            directionChunk.setVector3Value(sunLight->getDirections()[0]);
        } else {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string(light->getLightType()));
        }
    }

    void LightReaderWriter::loadPropertiesFrom(Light* light, const DataChunk* lightChunk, const DataParser& dataParser) {
        auto ambientColorChunk = dataParser.getUniqueChunk(true, AMBIENT_COLOR_TAG, UdaAttribute(), lightChunk);
        light->setAmbientColor(ambientColorChunk->getPoint3Value());
    }

    void LightReaderWriter::writePropertiesOn(DataChunk& lightChunk, const Light* light, UdaWriter& udaWriter) {
        auto& ambientColorChunk = udaWriter.createChunk(AMBIENT_COLOR_TAG, UdaAttribute(), &lightChunk);
        ambientColorChunk.setPoint3Value(light->getAmbientColor());
    }

    void LightReaderWriter::loadFlagsFrom(Light* light, const DataChunk* lightChunk, const DataParser& dataParser) {
        auto produceShadowChunk = dataParser.getUniqueChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightChunk);
        light->setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightReaderWriter::writeFlagsOn(DataChunk& lightChunk, const Light* light, UdaWriter& udaWriter) {
        auto& produceShadowChunk = udaWriter.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightChunk);
        produceShadowChunk.setBoolValue(light->isProduceShadow());
    }

}
