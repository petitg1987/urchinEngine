#include <stdexcept>

#include "tools/xml/XmlChunk.h"
#include "io/Converter.h"

namespace urchin
{

	XmlChunk::XmlChunk(TiXmlElement *chunk) :
			chunk(chunk)
	{

	}

	XmlChunk::XmlChunk(const TiXmlElement *chunk) :
			XmlChunk(const_cast<TiXmlElement *>(chunk))
	{

	}

	TiXmlElement *XmlChunk::getChunk() const
	{
		return chunk;
	}

	/**
	 * @return Attribute value if exist otherwise an empty string
	 */
	std::string XmlChunk::getAttributeValue(const std::string &attributeName) const
	{
		return *chunk->ToElement()->Attribute(attributeName);
	}

	void XmlChunk::setAttribute(const XmlAttribute &attribute)
	{
		chunk->SetAttribute(attribute.getAttributeName(), attribute.getAttributeValue());
	}

	std::string XmlChunk::getStringValue() const
	{
		if(chunk->FirstChild()==nullptr)
		{
			return "";
		}else if(chunk->FirstChild()->Type()==TiXmlNode::TEXT)
		{
			return chunk->FirstChild()->ToText()->Value();
		}

		throw std::domain_error("Impossible to find a value on chunk: " + chunk->ValueStr());
	}

	void XmlChunk::setStringValue(const std::string &value)
	{
		chunk->LinkEndChild(new TiXmlText(value));
	}

	int XmlChunk::getIntValue() const
	{
		return Converter::toInt(getStringValue());
	}

	void XmlChunk::setIntValue(int value)
	{
		setStringValue(std::to_string(value));
	}

	unsigned int XmlChunk::getUnsignedIntValue() const
	{
		return Converter::toUnsignedInt(getStringValue());
	}

	void XmlChunk::setUnsignedIntValue(unsigned int value)
	{
		setStringValue(std::to_string(value));
	}

	float XmlChunk::getFloatValue() const
	{
		return Converter::toFloat(getStringValue());
	}

	void XmlChunk::setFloatValue(float value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value));
	}

	bool XmlChunk::getBoolValue() const
	{
		std::string value = getStringValue();
		if(value == BOOL_FALSE)
		{
			return false;
		}else if(value == BOOL_TRUE)
		{
			return true;
		}

		throw std::domain_error("Impossible to convert " + value + " into a boolean");
	}

	void XmlChunk::setBoolValue(bool value)
	{
		if(value)
		{
			setStringValue(BOOL_TRUE);
		}else
		{
			setStringValue(BOOL_FALSE);
		}
	}

	char XmlChunk::getCharValue() const
	{
		return Converter::toChar(getStringValue());
	}

	void XmlChunk::setCharValue(char value)
	{
		setStringValue(std::to_string(value));
	}

	Point2<float> XmlChunk::getPoint2Value() const
	{
		return Converter::toPoint2(getStringValue());
	}

	void XmlChunk::setPoint2Value(const Point2<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y));
	}

	Point3<float> XmlChunk::getPoint3Value() const
	{
		return Converter::toPoint3(getStringValue());
	}

	void XmlChunk::setPoint3Value(const Point3<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z));
	}

	Point4<float> XmlChunk::getPoint4Value() const
	{
		return Converter::toPoint4(getStringValue());
	}

	void XmlChunk::setPoint4Value(const Point4<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z)
			+ FLOAT_DELIMITOR + std::to_string(value.W));
	}

	Vector2<float> XmlChunk::getVector2Value() const
	{
		return Converter::toVector2(getStringValue());
	}

	void XmlChunk::setVector2Value(const Vector2<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y));
	}

	Vector3<float> XmlChunk::getVector3Value() const
	{
		return Converter::toVector3(getStringValue());
	}

	void XmlChunk::setVector3Value(const Vector3<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z));
	}

	Vector4<float> XmlChunk::getVector4Value() const
	{
		return Converter::toVector4(getStringValue());
	}

	void XmlChunk::setVector4Value(const Vector4<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z)
			+ FLOAT_DELIMITOR + std::to_string(value.W));
	}
}
