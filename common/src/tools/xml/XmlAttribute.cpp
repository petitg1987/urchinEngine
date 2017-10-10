#include "tools/xml/XmlAttribute.h"

namespace urchin
{

	XmlAttribute::XmlAttribute() :
		attributeName(""),
		attributeValue("")
	{

	}

	XmlAttribute::XmlAttribute(const std::string &attributeName, const std::string &attributeValue) :
		attributeName(attributeName),
		attributeValue(attributeValue)
	{

	}

	const std::string &XmlAttribute::getAttributeName() const
	{
		return attributeName;
	}

	const std::string &XmlAttribute::getAttributeValue() const
	{
		return attributeValue;
	}

}
