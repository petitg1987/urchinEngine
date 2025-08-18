#include "map/save/object/light/LightReaderWriter.h"

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

            auto scopeRadiusChunk = udaParser.getFirstChunk(true, SCOPE_RADIUS_TAG, UdaAttribute(), lightChunk);
            auto falloffExponentChunk = udaParser.getFirstChunk(true, FALLOFF_EXPONENT_TAG, UdaAttribute(), lightChunk);
            omnidirectional->setScopeRadius(scopeRadiusChunk->getFloatValue());
            omnidirectional->setFalloffExponent(falloffExponentChunk->getFloatValue());

            return omnidirectional;
        } else if (lightType == SPOT_VALUE) {
            auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), lightChunk);
            auto directionChunk = udaParser.getFirstChunk(true, DIRECTION_TAG, UdaAttribute(), lightChunk);
            auto innerAngleChunk = udaParser.getFirstChunk(true, INNER_ANGLE_TAG, UdaAttribute(), lightChunk);
            auto outerAngleChunk = udaParser.getFirstChunk(true, OUTER_ANGLE_TAG, UdaAttribute(), lightChunk);
            auto spot = std::make_unique<SpotLight>(positionChunk->getPoint3Value(), directionChunk->getVector3Value(), innerAngleChunk->getFloatValue(), outerAngleChunk->getFloatValue());

            auto scopeRadiusChunk = udaParser.getFirstChunk(true, SCOPE_RADIUS_TAG, UdaAttribute(), lightChunk);
            auto falloffExponentChunk = udaParser.getFirstChunk(true, FALLOFF_EXPONENT_TAG, UdaAttribute(), lightChunk);
            spot->setScopeRadius(scopeRadiusChunk->getFloatValue());
            spot->setFalloffExponent(falloffExponentChunk->getFloatValue());

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

            auto& scopeRadiusChunk = udaParser.createChunk(SCOPE_RADIUS_TAG, UdaAttribute(), &lightChunk);
            scopeRadiusChunk.setFloatValue(omnidirectionalLight.getScopeRadius());

            auto& falloffExponentChunk = udaParser.createChunk(FALLOFF_EXPONENT_TAG, UdaAttribute(), &lightChunk);
            falloffExponentChunk.setFloatValue(omnidirectionalLight.getFalloffExponent());
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

            auto& scopeRadiusChunk = udaParser.createChunk(SCOPE_RADIUS_TAG, UdaAttribute(), &lightChunk);
            scopeRadiusChunk.setFloatValue(spotLight.getScopeRadius());

            auto& falloffExponentChunk = udaParser.createChunk(FALLOFF_EXPONENT_TAG, UdaAttribute(), &lightChunk);
            falloffExponentChunk.setFloatValue(spotLight.getFalloffExponent());
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

        auto lightMaskChunk = udaParser.getFirstChunk(true, LIGHT_MASK_TAG, UdaAttribute(), lightChunk);
        light.setLightMask(static_cast<uint8_t>(lightMaskChunk->getUnsignedIntValue()));

        auto produceShadowChunk = udaParser.getFirstChunk(true, PRODUCE_SHADOW_TAG, UdaAttribute(), lightChunk);
        light.setProduceShadow(produceShadowChunk->getBoolValue());

        auto shadowStrengthChunk = udaParser.getFirstChunk(true, SHADOW_STRENGTH_TAG, UdaAttribute(), lightChunk);
        light.setShadowStrength(shadowStrengthChunk->getFloatValue());
    }

    void LightReaderWriter::writeFlags(UdaChunk& lightChunk, const Light& light, UdaParser& udaParser) {
        auto& pbrEnabledChunk = udaParser.createChunk(PBR_ENABLED_TAG, UdaAttribute(), &lightChunk);
        pbrEnabledChunk.setBoolValue(light.isPbrEnabled());

        auto& lightMaskChunk = udaParser.createChunk(LIGHT_MASK_TAG, UdaAttribute(), &lightChunk);
        lightMaskChunk.setUnsignedIntValue(light.getLightMask());

        auto& produceShadowChunk = udaParser.createChunk(PRODUCE_SHADOW_TAG, UdaAttribute(), &lightChunk);
        produceShadowChunk.setBoolValue(light.isProduceShadow());

        auto& shadowStrengthChunk = udaParser.createChunk(SHADOW_STRENGTH_TAG, UdaAttribute(), &lightChunk);
        shadowStrengthChunk.setFloatValue(light.getShadowStrength());
    }

}
