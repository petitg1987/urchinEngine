#include "LightReaderWriter.h"

namespace urchin
{

	Light *LightReaderWriter::loadFrom(std::shared_ptr<XmlChunk> lightChunk, const XmlParser &xmlParser) const
	{
		Light *light = buildLightFrom(lightChunk, xmlParser);

		loadPropertiesOn(light, lightChunk, xmlParser);
		loadFlagsOn(light, lightChunk, xmlParser);

		return light;
	}

	void LightReaderWriter::writeOn(std::shared_ptr<XmlChunk> lightChunk, const Light *light, XmlWriter &xmlWriter) const
	{
		buildChunkFrom(lightChunk, light, xmlWriter);

		writePropertiesOn(lightChunk, light, xmlWriter);
		writeFlagsOn(lightChunk, light, xmlWriter);
	}

	Light *LightReaderWriter::buildLightFrom(const std::shared_ptr<XmlChunk> &lightChunk, const XmlParser &xmlParser) const
	{
		std::string lightType = lightChunk->getAttributeValue(TYPE_ATTR);
		if(lightType == OMNIDIRECTIONAL_VALUE)
		{
			std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), lightChunk);
			OmnidirectionalLight *omnidirectional = new OmnidirectionalLight(positionChunk->getPoint3Value());

			std::shared_ptr<XmlChunk> expenentialAttenuationChunk = xmlParser.getUniqueChunk(true, EXPONENTIAL_ATTENUATION_TAG, XmlAttribute(), lightChunk);
			omnidirectional->setAttenuation(expenentialAttenuationChunk->getFloatValue());

			return omnidirectional;
		}else if(lightType == SUN_VALUE)
		{
			std::shared_ptr<XmlChunk> directionChunk = xmlParser.getUniqueChunk(true, DIRECTION_TAG, XmlAttribute(), lightChunk);

			return new SunLight(directionChunk->getVector3Value());
		}

		throw std::invalid_argument("Unknown light type read from map: " + lightType);
	}

	void LightReaderWriter::buildChunkFrom(const std::shared_ptr<XmlChunk> &lightChunk, const Light *light, XmlWriter &xmlWriter) const
	{
		if(light->getLightType()==Light::OMNIDIRECTIONAL)
		{
			const auto *omnidirectionalLight = dynamic_cast<const OmnidirectionalLight *>(light);
			lightChunk->setAttribute(XmlAttribute(TYPE_ATTR, OMNIDIRECTIONAL_VALUE));

			std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), lightChunk);
			positionChunk->setPoint3Value(omnidirectionalLight->getPosition());

			std::shared_ptr<XmlChunk> expenentialAttenuationChunk = xmlWriter.createChunk(EXPONENTIAL_ATTENUATION_TAG, XmlAttribute(), lightChunk);
			expenentialAttenuationChunk->setFloatValue(omnidirectionalLight->getExponentialAttenuation());
		}else if(light->getLightType()==Light::SUN)
		{
			const auto *sunLight = dynamic_cast<const SunLight *>(light);
			lightChunk->setAttribute(XmlAttribute(TYPE_ATTR, SUN_VALUE));

			std::shared_ptr<XmlChunk> directionChunk = xmlWriter.createChunk(DIRECTION_TAG, XmlAttribute(), lightChunk);
			directionChunk->setVector3Value(sunLight->getDirections()[0]);
		}else
		{
			throw std::invalid_argument("Unknown light type to write in map: " + std::to_string(light->getLightType()));
		}
	}

	void LightReaderWriter::loadPropertiesOn(Light *light, const std::shared_ptr<XmlChunk> &lightChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> ambientColorChunk = xmlParser.getUniqueChunk(true, AMBIENT_COLOR_TAG, XmlAttribute(), lightChunk);
		light->setAmbientColor(ambientColorChunk->getPoint3Value());
	}

	void LightReaderWriter::writePropertiesOn(const std::shared_ptr<XmlChunk> &lightChunk, const Light *light, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> ambientColorChunk = xmlWriter.createChunk(AMBIENT_COLOR_TAG, XmlAttribute(), lightChunk);
		ambientColorChunk->setPoint3Value(light->getAmbientColor());
	}

	void LightReaderWriter::loadFlagsOn(Light *light, const std::shared_ptr<XmlChunk> &lightChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> produceShadowChunk = xmlParser.getUniqueChunk(true, PRODUCE_SHADOW_TAG, XmlAttribute(), lightChunk);
		light->setProduceShadow(produceShadowChunk->getBoolValue());
	}

	void LightReaderWriter::writeFlagsOn(const std::shared_ptr<XmlChunk> &lightChunk, const Light *light, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> produceShadowChunk = xmlWriter.createChunk(PRODUCE_SHADOW_TAG, XmlAttribute(), lightChunk);
		produceShadowChunk->setBoolValue(light->isProduceShadow());
	}

}
