#include "LightReaderWriter.h"

namespace urchin {

    Light* LightReaderWriter::loadFrom(const XmlChunk* lightChunk, const XmlParser& xmlParser) {
        Light* light = buildLightFrom(lightChunk, xmlParser);

        loadPropertiesFrom(light, lightChunk, xmlParser);
        loadFlagsFrom(light, lightChunk, xmlParser);

        return light;
    }

    void LightReaderWriter::writeOn(XmlChunk* lightChunk, const Light* light, XmlWriter& xmlWriter) {
        buildChunkFrom(lightChunk, light, xmlWriter);

        writePropertiesOn(lightChunk, light, xmlWriter);
        writeFlagsOn(lightChunk, light, xmlWriter);
    }

    Light* LightReaderWriter::buildLightFrom(const XmlChunk* lightChunk, const XmlParser& xmlParser) {
        std::string lightType = lightChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), lightChunk);
            auto* omnidirectional = new OmnidirectionalLight(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = xmlParser.getUniqueChunk(true, EXPONENTIAL_ATTENUATION_TAG, DataAttribute(), lightChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SUN_VALUE) {
            auto directionChunk = xmlParser.getUniqueChunk(true, DIRECTION_TAG, DataAttribute(), lightChunk);

            return new SunLight(directionChunk->getVector3Value());
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightReaderWriter::buildChunkFrom(XmlChunk* lightChunk, const Light* light, XmlWriter& xmlWriter) {
        if (light->getLightType() == Light::OMNIDIRECTIONAL) {
            const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light);
            lightChunk->setAttribute(DataAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto positionChunk = xmlWriter.createChunk(POSITION_TAG, DataAttribute(), lightChunk);
            positionChunk->setPoint3Value(omnidirectionalLight->getPosition());

            auto exponentialAttenuationChunk = xmlWriter.createChunk(EXPONENTIAL_ATTENUATION_TAG, DataAttribute(), lightChunk);
            exponentialAttenuationChunk->setFloatValue(omnidirectionalLight->getExponentialAttenuation());
        } else if (light->getLightType() == Light::SUN) {
            const auto* sunLight = dynamic_cast<const SunLight*>(light);
            lightChunk->setAttribute(DataAttribute(TYPE_ATTR, SUN_VALUE));

            auto directionChunk = xmlWriter.createChunk(DIRECTION_TAG, DataAttribute(), lightChunk);
            directionChunk->setVector3Value(sunLight->getDirections()[0]);
        } else {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string(light->getLightType()));
        }
    }

    void LightReaderWriter::loadPropertiesFrom(Light* light, const XmlChunk* lightChunk, const XmlParser& xmlParser) {
        auto ambientColorChunk = xmlParser.getUniqueChunk(true, AMBIENT_COLOR_TAG, DataAttribute(), lightChunk);
        light->setAmbientColor(ambientColorChunk->getPoint3Value());
    }

    void LightReaderWriter::writePropertiesOn(const XmlChunk* lightChunk, const Light* light, XmlWriter& xmlWriter) {
        auto ambientColorChunk = xmlWriter.createChunk(AMBIENT_COLOR_TAG, DataAttribute(), lightChunk);
        ambientColorChunk->setPoint3Value(light->getAmbientColor());
    }

    void LightReaderWriter::loadFlagsFrom(Light* light, const XmlChunk* lightChunk, const XmlParser& xmlParser) {
        auto produceShadowChunk = xmlParser.getUniqueChunk(true, PRODUCE_SHADOW_TAG, DataAttribute(), lightChunk);
        light->setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightReaderWriter::writeFlagsOn(const XmlChunk* lightChunk, const Light* light, XmlWriter& xmlWriter) {
        auto produceShadowChunk = xmlWriter.createChunk(PRODUCE_SHADOW_TAG, DataAttribute(), lightChunk);
        produceShadowChunk->setBoolValue(light->isProduceShadow());
    }

}
