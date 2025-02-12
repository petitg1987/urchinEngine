#include <map/save/object/light/LightEntityReaderWriter.h>

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

    void LightEntityReaderWriter::write(UdaChunk& lightEntityChunk, const LightEntity& lightEntity, UdaParser& udaParser) {
        lightEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, lightEntity.getName()));

        writeLightChunk(lightEntityChunk, *lightEntity.getLight(), udaParser);
        writeProperties(lightEntityChunk, *lightEntity.getLight(), udaParser);
        writeFlags(lightEntityChunk, *lightEntity.getLight(), udaParser);
    }

    std::unique_ptr<Light> LightEntityReaderWriter::buildLight(const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        std::string lightType = lightEntityChunk->getAttributeValue(TYPE_ATTR);
        if (lightType == SUN_VALUE) {
            auto directionChunk = udaParser.getFirstChunk(true, DIRECTION_TAG, UdaAttribute(), lightEntityChunk);

            return std::make_unique<SunLight>(directionChunk->getVector3Value());
        } else if (lightType == OMNIDIRECTIONAL_VALUE) {
            auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), lightEntityChunk);
            auto omnidirectional = std::make_unique<OmnidirectionalLight>(positionChunk->getPoint3Value());

            auto exponentialAttenuationChunk = udaParser.getFirstChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightEntityChunk);
            omnidirectional->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SPOT_VALUE) {
            auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), lightEntityChunk);
            auto directionChunk = udaParser.getFirstChunk(true, DIRECTION_TAG, UdaAttribute(), lightEntityChunk);
            auto innerAngleChunk = udaParser.getFirstChunk(true, INNER_ANGLE_TAG, UdaAttribute(), lightEntityChunk);
            auto outerAngleChunk = udaParser.getFirstChunk(true, OUTER_ANGLE_TAG, UdaAttribute(), lightEntityChunk);
            auto spot = std::make_unique<SpotLight>(positionChunk->getPoint3Value(), directionChunk->getVector3Value(), innerAngleChunk->getFloatValue(), outerAngleChunk->getFloatValue());

            auto exponentialAttenuationChunk = udaParser.getFirstChunk(true, EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), lightEntityChunk);
            spot->setAttenuation(exponentialAttenuationChunk->getFloatValue());

            return spot;
        }

        throw std::invalid_argument("Unknown light type read from map: " + lightType);
    }

    void LightEntityReaderWriter::writeLightChunk(UdaChunk& lightEntityChunk, const Light& light, UdaParser& udaParser) {
        if (light.getLightType() == Light::LightType::SUN) {
            const auto& sunLight = static_cast<const SunLight&>(light);
            lightEntityChunk.addAttribute(UdaAttribute(TYPE_ATTR, SUN_VALUE));

            auto& directionChunk = udaParser.createChunk(DIRECTION_TAG, UdaAttribute(), &lightEntityChunk);
            directionChunk.setVector3Value(sunLight.getDirections()[0]);
        } else if (light.getLightType() == Light::LightType::OMNIDIRECTIONAL) {
            const auto& omnidirectionalLight = static_cast<const OmnidirectionalLight&>(light);
            lightEntityChunk.addAttribute(UdaAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

            auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &lightEntityChunk);
            positionChunk.setPoint3Value(omnidirectionalLight.getPosition());

            auto& exponentialAttenuationChunk = udaParser.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightEntityChunk);
            exponentialAttenuationChunk.setFloatValue(omnidirectionalLight.getExponentialAttenuation());
        } else if (light.getLightType() == Light::LightType::SPOT) {
            const auto& spotLight = static_cast<const SpotLight&>(light);
            lightEntityChunk.addAttribute(UdaAttribute(TYPE_ATTR, SPOT_VALUE));

            auto& positionChunk = udaParser.createChunk(POSITION_TAG, UdaAttribute(), &lightEntityChunk);
            positionChunk.setPoint3Value(spotLight.getPosition());

            auto& directionChunk = udaParser.createChunk(DIRECTION_TAG, UdaAttribute(), &lightEntityChunk);
            directionChunk.setVector3Value(spotLight.getDirections()[0]);

            auto& innerAngleChunk = udaParser.createChunk(INNER_ANGLE_TAG, UdaAttribute(), &lightEntityChunk);
            innerAngleChunk.setFloatValue(spotLight.getInnerAngle());

            auto& outerAngleChunk = udaParser.createChunk(OUTER_ANGLE_TAG, UdaAttribute(), &lightEntityChunk);
            outerAngleChunk.setFloatValue(spotLight.getOuterAngle());

            auto& exponentialAttenuationChunk = udaParser.createChunk(EXPONENTIAL_ATTENUATION_TAG, UdaAttribute(), &lightEntityChunk);
            exponentialAttenuationChunk.setFloatValue(spotLight.getExponentialAttenuation());
        } else  {
            throw std::invalid_argument("Unknown light type to write in map: " + std::to_string((int)light.getLightType()));
        }
    }

    void LightEntityReaderWriter::loadProperties(Light& light, const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        auto colorChunk = udaParser.getFirstChunk(true, LIGHT_COLOR_TAG, UdaAttribute(), lightEntityChunk);
        light.setLightColor(colorChunk->getPoint3Value());
    }

    void LightEntityReaderWriter::writeProperties(UdaChunk& lightEntityChunk, const Light& light, UdaParser& udaParser) {
        auto& colorChunk = udaParser.createChunk(LIGHT_COLOR_TAG, UdaAttribute(), &lightEntityChunk);
        colorChunk.setPoint3Value(light.getLightColor());
    }

    void LightEntityReaderWriter::loadFlags(Light& light, const UdaChunk* lightEntityChunk, const UdaParser& udaParser) {
        auto pbrEnabledChunk = udaParser.getFirstChunk(true, PBR_ENABLED_TAG, UdaAttribute(), lightEntityChunk);
        light.setPbrEnabled(pbrEnabledChunk->getBoolValue());

        auto produceShadowChunk = udaParser.getFirstChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightEntityChunk);
        light.setProduceShadow(produceShadowChunk->getBoolValue());
    }

    void LightEntityReaderWriter::writeFlags(UdaChunk& lightEntityChunk, const Light& light, UdaParser& udaParser) {
        auto& pbrEnabledChunk = udaParser.createChunk(PBR_ENABLED_TAG, UdaAttribute(), &lightEntityChunk);
        pbrEnabledChunk.setBoolValue(light.isPbrEnabled());

        auto& produceShadowChunk = udaParser.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightEntityChunk);
        produceShadowChunk.setBoolValue(light.isProduceShadow());
    }

}
