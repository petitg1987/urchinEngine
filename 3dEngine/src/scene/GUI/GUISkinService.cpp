#include <memory>
#include <stdexcept>

#include "scene/GUI/GUISkinService.h"
#include "resources/MediaManager.h"

namespace urchin
{

	GUISkinService::GUISkinService() : Singleton<GUISkinService>(), xmlSkin(nullptr)
	{

	}

	GUISkinService::~GUISkinService()
	{
		delete xmlSkin;
	}

	void GUISkinService::setSkin(const std::string &skinFilename)
	{
		delete xmlSkin;

		xmlSkin = new XmlParser(skinFilename);
	}

	std::shared_ptr<Image> GUISkinService::createTexWidget(unsigned int width, unsigned int height, std::shared_ptr<XmlChunk> skinXmlChunk, WidgetOutline *widgetOutline) const
	{
		//skin informations
		std::shared_ptr<XmlChunk> imgWidgetElem = getXmlSkin()->getUniqueChunk(true, "image", XmlAttribute(), skinXmlChunk);
		Image *imgWidget = MediaManager::instance()->getMedia<Image>(imgWidgetElem->getStringValue());

		std::shared_ptr<XmlChunk> topElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "top"), skinXmlChunk));
		unsigned int top = topElem->getIntValue();

		std::shared_ptr<XmlChunk> bottomElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "bottom"), skinXmlChunk));
		unsigned int bottom = bottomElem->getIntValue();

		std::shared_ptr<XmlChunk> leftElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "left"), skinXmlChunk));
		unsigned int left = leftElem->getIntValue();

		std::shared_ptr<XmlChunk> rightElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "right"), skinXmlChunk));
		unsigned int right = rightElem->getIntValue();

		//copy the informations into the outline
		if(widgetOutline)
		{
			widgetOutline->topWidth = top;
			widgetOutline->bottomWidth = bottom;
			widgetOutline->leftWidth = left;
			widgetOutline->rightWidth = right;
		}

		//creates the image width*height
		int internalFormat = imgWidget->getInternalFormat(); //shortcut
		unsigned char *texels = new unsigned char[height*width*internalFormat];

		unsigned int widthMinusRight = std::max(0, (int)width-(int)right);
		unsigned int heightMinusBottom = std::max(0, (int)height-(int)bottom);
		unsigned int leftMultiplyInternalFormat = std::min(width, left*internalFormat);
		unsigned int topAdjusted = std::min(height, top);

		//copy corner top left
		for(unsigned int i=0;i<topAdjusted;++i)
		{
			for(unsigned int j=0;j<leftMultiplyInternalFormat;++j)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*internalFormat + j];
			}
		}

		//copy top
		for(unsigned int i=0;i<topAdjusted;++i)
		{
			for(unsigned int j=left*internalFormat, k=0;j<widthMinusRight*internalFormat;++j, ++k)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*internalFormat + k%((imgWidget->getWidth()-(right+left))*internalFormat) + left*internalFormat];
			}
		}

		//copy corner top right
		for(unsigned int i=0;i<topAdjusted;++i)
		{
			for(unsigned int j=widthMinusRight*internalFormat, k=(imgWidget->getWidth()-right)*internalFormat;j<width*internalFormat;++j, ++k)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*internalFormat + k];
			}
		}

		//copy right
		for(unsigned int i=top, k=0;i<heightMinusBottom;++i, ++k)
		{
			for(unsigned int j=widthMinusRight*internalFormat, l=(imgWidget->getWidth()-right)*internalFormat;j<width*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*internalFormat + l];
			}
		}

		//copy corner bottom right
		for(unsigned int i=heightMinusBottom, k=imgWidget->getHeight()-bottom;i<height;++i, ++k)
		{
			for(unsigned int j=widthMinusRight*internalFormat, l=(imgWidget->getWidth()-right)*internalFormat;j<width*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[k*imgWidget->getWidth()*internalFormat + l];
			}
		}

		//copy bottom
		for(unsigned int i=heightMinusBottom, k=imgWidget->getHeight()-bottom;i<height;++i, ++k)
		{
			for(unsigned int j=left*internalFormat, l=0;j<widthMinusRight*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[k*imgWidget->getWidth()*internalFormat + l%((imgWidget->getWidth()-(left+right))*internalFormat) + left*internalFormat];
			}
		}

		//copy corner bottom left
		for(unsigned int i=heightMinusBottom, k=0;i<height;++i, ++k)
		{
			for(unsigned int j=0;j<leftMultiplyInternalFormat;++j)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k + imgWidget->getHeight()-bottom)*imgWidget->getWidth()*internalFormat + j];
			}
		}

		//copy left
		for(unsigned int i=top, k=0;i<heightMinusBottom;++i, ++k)
		{
			for(unsigned int j=0;j<leftMultiplyInternalFormat;++j)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*internalFormat + j];
			}
		}

		//copy middle
		for(unsigned int i=top, k=0;i<heightMinusBottom;++i, ++k)
		{
			for(unsigned int j=leftMultiplyInternalFormat, l=0;j<widthMinusRight*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*internalFormat + l%((imgWidget->getWidth()-(left+right))*internalFormat) + left*internalFormat];
			}
		}

		//create the texture
		std::shared_ptr<Image> texWidget = std::shared_ptr<Image>(new Image(imgWidget->getInternalFormat(), width, height, imgWidget->getFormat(), texels), Resource::ResourceDeleter());
		texWidget->toTexture(false, false);

		imgWidget->release();

		return texWidget;
	}

	XmlParser *GUISkinService::getXmlSkin() const
	{
		if(xmlSkin==nullptr)
		{
			throw std::runtime_error("Skin for the GUI isn't initialized.");
		}

		return xmlSkin;
	}

}
