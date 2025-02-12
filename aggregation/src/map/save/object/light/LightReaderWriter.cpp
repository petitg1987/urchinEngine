#include <map/save/object/light/LightReaderWriter.h>

namespace urchin {

    std::unique_ptr<Light> LightReaderWriter::load(const UdaChunk* lightChunk, const UdaParser& udaParser) {
        std::unique_ptr<Light> light = buildLight(lightChunk, udaParser);
        loadProperties(*light, lightChunk, udaParser);
        loadFlags(*light, lightChunk, udaParser);
        return light;
    }

    void LightReaderWriter::write(UdaChunk& lightChunk, const Light& light, UdaParser& udaParser) {
        writeLightChunk(lightChunk, light, udaParser);
        writeProperties(lightChunk, light, udaParser);
        writeFlags(lightChunk, light, udaParser);
    }

    std::unique_ptr<Light> LightReaderWriter::buildLight(const UdaChunk* lightChunk, const UdaParser& udaParser) {
        std::string lightType = lightChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == SUN_VALUE) {
            auto directionChunk = udaParser.getFirstChunk(true, DIRECTION_TAG, UdaAttribute(), lightChunk);

            return std::make_unique<SunLight>(directionChunk->getVector3Value());
        } else if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), lightChunk);
            auto omnidirectional = std::make_unique<OmnidirectionalLight>(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = udaParser.getFirstChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SPOT_VALUE) {
            auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), lightChunk);
            auto directionChunk = udaParser.getFirstChunk(true, DIRECTION_TAG, UdaAttribute(), lightChunk);
            auto innerAngleChunk = udaParser.getFirstChunk(true, INNER_ANGLE_TAG, UdaAttribute(), lightChunk);
            auto outerAngleChunk = udaParser.getFirstChunk(true, OUTER_ANGLE_TAG, UdaAttribute(), lightChunk);
            auto spot = std::make_unique<SpotLight>(positionChunk->getPoint3Value(), directionChunk->getVector3Value(), innerAngleChunk->getFloatValue(), outerAngleChunk->getFloatValue());

            auto exponentialAttenuationChunk = udaParser.getFirstChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightChunk);
            spot->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return spot;
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightReaderWriter::writeLightChunk(UdaChunk& lightChunk, const Light& light, UdaParser& udaParser) {
        if (light.getLightType() == Light::LightType::SUN) {
            const auto& sunLight = static_cast<const SunLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, SUN_VALUE));

            auto& directionChunk = udaParser.createChunk(DIRECTION_TAG, UdaAttribute(), &lightChunk);
            directionChunk.setVector3Value(sunLight.getDirections()[0]);
        } else if (light.getLightType() == Light::LightType::OMNIDIRECTIONAL) {
            const auto& omnidirectionalLight = static_cast<const OmnidirectionalLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &lightChunk);
            positionChunk.setPoint3Value(omnidirectionalLight.getPosition());

            auto& exponentialAttenuationChunk = udaParser.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightChunk);
            exponentialAttenuationChunk.setFloatValue(omnidirectionalLight.getExponentialAttenuation());
        } else if (light.getLightType() == Light::LightType::SPOT) {
            const auto& spotLight = static_cast<const SpotLight&>(light);
            lightChunk.addAttribute(UdaAttribute(TYPE_ATTR, SPOT_VALUE));

            auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &lightChunk);
            positionChunk.setPoint3Value(spotLight.getPosition());

            auto& directionChunk = udaParser.createChunk(DIRECTION_TAG, UdaAttribute(), &lightChunk);
            directionChunk.setVector3Value(spotLight.getDirections()[0]);

            auto& innerAngleChunk = udaParser.createChunk(INNER_ANGLE_TAG, UdaAttribute(), &lightChunk);
            innerAngleChunk.setFloatValue(spotLight.getInnerAngle());

            auto& outerAngleChunk = udaParser.createChunk(OUTER_ANGLE_TAG, UdaAttribute(), &lightChunk);
            outerAngleChunk.setFloatValue(spotLight.getOuterAngle());

            auto& exponentialAttenuationChunk = udaParser.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightChunk);
            exponentialAttenuationChunk.setFloatValue(spotLight.getExponentialAttenuation());
        } else  {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string((int)light.getLightType()));
        }
    }

    void LightReaderWriter::loadProperties(Light& light, const UdaChunk* lightChunk, const UdaParser& udaParser) {
        auto colorChunk = udaParser.getFirstChunk(true, LIGHT_COLOR_TAG, UdaAttribute(), lightChunk);
        light.setLightColor(colorChunk->getPoint3Value());
    }

    void LightReaderWriter::writeProperties(UdaChunk& lightChunk, const Light& light, UdaParser& udaParser) {
        auto& colorChunk = udaParser.createChunk(LIGHT_COLOR_TAG, UdaAttribute(), &lightChunk);
        colorChunk.setPoint3Value(light.getLightColor());
    }

    void LightReaderWriter::loadFlags(Light& light, const UdaChunk* lightChunk, const UdaParser& udaParser) {
        auto pbrEnabledChunk = udaParser.getFirstChunk(true, PBR_ENABLED_TAG, UdaAttribute(), lightChunk);
        light.setPbrEnabled(pbrEnabledChunk->getBoolValue());

        auto produceShadowChunk = udaParser.getFirstChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightChunk);
        light.setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightReaderWriter::writeFlags(UdaChunk& lightChunk, const Light& light, UdaParser& udaParser) {
        auto& pbrEnabledChunk = udaParser.createChunk(PBR_ENABLED_TAG, UdaAttribute(), &lightChunk);
        pbrEnabledChunk.setBoolValue(light.isPbrEnabled());

        auto& produceShadowChunk = udaParser.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightChunk);
        produceShadowChunk.setBoolValue(light.isProduceShadow());
    }

}
