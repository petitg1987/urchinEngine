#include "tools/xml/XmlAttribute.h"

namespace urchin
{

	/**
	 * Constructor
	 */
	XmlAttribute::XmlAttribute() :
		attributeName(""),
		attributeValue("")
	{

	}

	/**
	 * Constructor
	 * @param attributeName Name of the attribute
	 * @param attributeValue Value of the attribute
	 */
	XmlAttribute::XmlAttribute(const std::string &attributeName, const std::string &attributeValue) :
		attributeName(attributeName),
		attributeValue(attributeValue)
	{

	}

	/**
	 * Returns the attribute name
	 * @return Attribute name
	 */
	const std::string &XmlAttribute::getAttributeName() const
	{
		return attributeName;
	}

	/**
	 * Returns the attribute value
	 * @return Attribute value
	 */
	const std::string &XmlAttribute::getAttributeValue() const
	{
		return attributeValue;
	}

}
