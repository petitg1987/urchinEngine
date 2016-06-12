#include <stdexcept>

#include "tools/xml/XmlChunk.h"
#include "io/Converter.h"

namespace urchin
{

	/**
	 * Constructor
	 * @param chunk XML chunk
	 */
	XmlChunk::XmlChunk(TiXmlElement *chunk) :
			chunk(chunk)
	{

	}

	/**
	 * Constructor
	 * @param chunk XML chunk
	 */
	XmlChunk::XmlChunk(const TiXmlElement *chunk) :
			XmlChunk(const_cast<TiXmlElement *>(chunk))
	{

	}

	/**
	 * Returns the Xml chunk
	 * @return Xml chunk
	 */
	TiXmlElement *XmlChunk::getChunk() const
	{
		return chunk;
	}

	/**
	 * Returns attribute value
	 * @param Attribute name
	 * @return Attribute value if exist otherwise an empty string
	 */
	std::string XmlChunk::getAttributeValue(const std::string &attributeName) const
	{
		return *chunk->ToElement()->Attribute(attributeName);
	}

	/**
	 * Sets attribute value
	 * @param attribute Attribute to set
	 */
	void XmlChunk::setAttribute(const XmlAttribute &attribute)
	{
		chunk->SetAttribute(attribute.getAttributeName(), attribute.getAttributeValue());
	}

	/**
	 * Returns chunk value in string
	 * @return Chunk value in string
	 */
	std::string XmlChunk::getStringValue() const
	{
		if(chunk->FirstChild()->Type()==TiXmlNode::TEXT)
		{
			return chunk->FirstChild()->ToText()->Value();
		}

		throw std::domain_error("Impossible to find a value on chunk: " + chunk->ValueStr());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setStringValue(const std::string &value)
	{
		chunk->LinkEndChild(new TiXmlText(value));
	}

	/**
	 * Returns chunk value in integer
	 * @return Chunk value in integer
	 */
	int XmlChunk::getIntValue() const
	{
		return Converter::toInt(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setIntValue(int value)
	{
		setStringValue(std::to_string(value));
	}

	/**
	 * Returns chunk value in float
	 * @return Chunk value in float
	 */
	float XmlChunk::getFloatValue() const
	{
		return Converter::toFloat(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setFloatValue(float value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value));
	}

	/**
	 * Returns chunk value in boolean
	 * @return Chunk value in boolean
	 */
	bool XmlChunk::getBoolValue() const
	{
		std::string value = getStringValue();
		if(value.compare(BOOL_FALSE)==0)
		{
			return false;
		}else if(value.compare(BOOL_TRUE)==0)
		{
			return true;
		}

		throw std::domain_error("Impossible to convert " + value + " into a boolean");
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
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

	/**
	 * Returns chunk value in char
	 * @return Chunk value in char
	 */
	char XmlChunk::getCharValue() const
	{
		return Converter::toChar(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setCharValue(char value)
	{
		setStringValue(std::to_string(value));
	}

	/**
	 * Returns chunk value in point 2
	 * @return Chunk value in point 2
	 */
	Point2<float> XmlChunk::getPoint2Value() const
	{
		return Converter::toPoint2(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setPoint2Value(const Point2<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y));
	}

	/**
	 * Returns chunk value in point 3
	 * @return Chunk value in point 3
	 */
	Point3<float> XmlChunk::getPoint3Value() const
	{
		return Converter::toPoint3(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setPoint3Value(const Point3<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z));
	}

	/**
	 * Returns chunk value in point 4
	 * @return Chunk value in point 4
	 */
	Point4<float> XmlChunk::getPoint4Value() const
	{
		return Converter::toPoint4(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setPoint4Value(const Point4<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z)
			+ FLOAT_DELIMITOR + std::to_string(value.W));
	}

	/**
	 * Returns chunk value in vector 2
	 * @return Chunk value in vector 2
	 */
	Vector2<float> XmlChunk::getVector2Value() const
	{
		return Converter::toVector2(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setVector2Value(const Vector2<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y));
	}

	/**
	 * Returns chunk value in vector 3
	 * @return Chunk value in vector 3
	 */
	Vector3<float> XmlChunk::getVector3Value() const
	{
		return Converter::toVector3(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setVector3Value(const Vector3<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z));
	}

	/**
	 * Returns chunk value in vector 4
	 * @return Chunk value in vector 4
	 */
	Vector4<float> XmlChunk::getVector4Value() const
	{
		return Converter::toVector4(getStringValue());
	}

	/**
	 * Sets value on chunk
	 * @param value Value to set
	 */
	void XmlChunk::setVector4Value(const Vector4<float> &value)
	{
		std::locale::global(std::locale("C")); //for float

		setStringValue(std::to_string(value.X) + FLOAT_DELIMITOR + std::to_string(value.Y) + FLOAT_DELIMITOR + std::to_string(value.Z)
			+ FLOAT_DELIMITOR + std::to_string(value.W));
	}
}
