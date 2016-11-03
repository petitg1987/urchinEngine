#include <stdexcept>

#include "tools/xml/XmlParser.h"
#include "system/FileSystem.h"

namespace urchin
{

	/**
	 * Constructor
	 * @param filename XML filename
	 */
	XmlParser::XmlParser(const std::string &filename) :
		XmlParser(filename, FileSystem::instance()->getResourcesDirectory())
	{

	}

	/**
	 * Constructor
	 * @param filename XML filename
	 * @param workingDirectory Override the default working directory
	 */
	XmlParser::XmlParser(const std::string &filename, const std::string &workingDirectory)
	{
		this->filenamePath = workingDirectory + filename;
		this->doc = new TiXmlDocument(filenamePath.c_str());
		
		if(!doc->LoadFile())
		{
			delete doc;
			throw std::invalid_argument("Cannot open or load the file " + filenamePath + ".");
		}
	}

	/**
	 * Destructor
	 */
	XmlParser::~XmlParser()
	{
		delete doc;
	}

	/**
	 * Return a XML chunk representing the root
	 * @return XML chunk representing the root
	 */
	std::shared_ptr<XmlChunk> XmlParser::getRootChunk() const
	{
		const TiXmlNode *rootNode = doc->FirstChild()->NextSibling();
		if(rootNode->Type()==TiXmlNode::ELEMENT)
		{
			return std::make_shared<XmlChunk>(doc->FirstChild()->NextSibling()->ToElement());
		}

		throw std::invalid_argument("Impossible to get root element in file: " + filenamePath);
	}

	/**
	 * Returns a XML chunks according to the parameters (info: <parent><chunkName attributeName="attributeValue">value</chunkName></parent>)
	 * @param chunkName Name of the tag XML
	 * @param attribute Name and value of the attribute
	 * @param parent Name of the tag parent of "chunkName"
	 * @return XML chunks according to the parameters
	 */
	std::vector<std::shared_ptr<XmlChunk>> XmlParser::getChunks(const std::string &chunkName, const XmlAttribute &attribute, std::shared_ptr<XmlChunk> parent) const
	{
		std::vector<std::shared_ptr<XmlChunk>> chunks;

		const TiXmlNode *firstChild;
		if(parent.get()==nullptr)
		{
			firstChild = doc->FirstChild()->NextSibling()->FirstChild();
		}else
		{
			firstChild = parent->getChunk()->FirstChild();
		}

		//seek the good chunk
		for (const TiXmlNode *pChild=firstChild; pChild!=nullptr; pChild=pChild->NextSibling()) 
		{
			if(pChild->Type()==TiXmlNode::ELEMENT && pChild->Value()==chunkName)
			{
				if(!attribute.getAttributeName().empty())
				{
					const std::string *attributeValue = pChild->ToElement()->Attribute(attribute.getAttributeName());
					if(attributeValue!=nullptr && (*attributeValue).compare(attribute.getAttributeValue())==0)
					{
						chunks.push_back(std::make_shared<XmlChunk>(pChild->ToElement()));
					}
				}else
				{
					chunks.push_back(std::make_shared<XmlChunk>(pChild->ToElement()));
				}
			}
		}
		
		return chunks;
	}

	/**
	 * Returns a unique XML chunk according to the parameters (info: <parent><chunkName attributeName="attributeValue">value</chunkName></parent>)
	 * @param mandatory Chunk must exist otherwise an exception is throw
	 * @param chunkName Name of the tag XML
	 * @param attribute Name and value of the attribute
	 * @param parent Name of the tag parent of "chunkName"
	 * @return Unique XML chunk according to the parameters
	 */
	std::shared_ptr<XmlChunk> XmlParser::getUniqueChunk(bool mandatory, const std::string &chunkName, const XmlAttribute &attribute, std::shared_ptr<XmlChunk> parent) const
	{
		std::vector<std::shared_ptr<XmlChunk>> chunks = getChunks(chunkName, attribute, parent);

		if(chunks.size()>1)
		{
			throw std::invalid_argument("More than one tag found for " + getChunkDescription(chunkName, attribute) + ".");
		}else if(chunks.size()==0)
		{
			if(mandatory)
			{
				throw std::invalid_argument("The tag " + getChunkDescription(chunkName, attribute) + " wasn't found in file " + filenamePath + ".");
			}

			return std::shared_ptr<XmlChunk>(nullptr);
		}

		return chunks[0];
	}

	/**
	 * Returns a description of the chunk
	 * @param chunkName Name of the tag XML
	 * @param attribute Name and value of the attribute
	 * @return Description of the chunk
	 */
	std::string XmlParser::getChunkDescription(const std::string &chunkName, const XmlAttribute &attribute) const
	{
		if(attribute.getAttributeName().empty())
		{
			return chunkName;
		}else
		{
			return chunkName + " with the attribute " + attribute.getAttributeName() + "=\"" + attribute.getAttributeValue() + "\"";
		}
	}

}
