#include <stdexcept>
#include <memory>

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

	std::shared_ptr<Image> GUISkinService::createTexWidget(unsigned int width, unsigned int height, const std::shared_ptr<XmlChunk> &skinXmlChunk, WidgetOutline *widgetOutline) const
	{
		//skin information
		std::shared_ptr<XmlChunk> imgWidgetElem = getXmlSkin()->getUniqueChunk(true, "image", XmlAttribute(), skinXmlChunk);
		auto *imgWidget = MediaManager::instance()->getMedia<Image>(imgWidgetElem->getStringValue());

		std::shared_ptr<XmlChunk> topElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "top"), skinXmlChunk));
		unsigned int top = topElem->getUnsignedIntValue();

		std::shared_ptr<XmlChunk> bottomElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "bottom"), skinXmlChunk));
		unsigned int bottom = bottomElem->getUnsignedIntValue();

		std::shared_ptr<XmlChunk> leftElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "left"), skinXmlChunk));
		unsigned int left = leftElem->getUnsignedIntValue();

		std::shared_ptr<XmlChunk> rightElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "right"), skinXmlChunk));
		unsigned int right = rightElem->getUnsignedIntValue();

		//copy the information into the outline
		if(widgetOutline)
		{
			widgetOutline->topWidth = top;
			widgetOutline->bottomWidth = bottom;
			widgetOutline->leftWidth = left;
			widgetOutline->rightWidth = right;
		}

		//creates the image width*height
		unsigned int componentsCount = imgWidget->retrieveComponentsCount(); //shortcut
		std::vector<unsigned char> texels(height*width*componentsCount, 0);

		unsigned int widthMinusRight = static_cast<unsigned int>(std::max(0, (int)width-(int)right));
		unsigned int heightMinusBottom = static_cast<unsigned int>(std::max(0, (int)height-(int)bottom));
		unsigned int leftMultiplyInternalFormat = std::min(width, left*componentsCount);
		unsigned int topAdjusted = std::min(height, top);

		//copy corner top left
		for(unsigned int i=0;i<topAdjusted;++i)
		{
			for(unsigned int j=0;j<leftMultiplyInternalFormat;++j)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*componentsCount + j];
			}
		}

		//copy top
		for(unsigned int i=0;i<topAdjusted;++i)
		{
			for(unsigned int j=left*componentsCount, k=0;j<widthMinusRight*componentsCount;++j, ++k)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*componentsCount + k%((imgWidget->getWidth()-(right+left))*componentsCount) + left*componentsCount];
			}
		}

		//copy corner top right
		for(unsigned int i=0;i<topAdjusted;++i)
		{
			for(unsigned int j=widthMinusRight*componentsCount, k=(imgWidget->getWidth()-right)*componentsCount;j<width*componentsCount;++j, ++k)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*componentsCount + k];
			}
		}

		//copy right
		for(unsigned int i=top, k=0;i<heightMinusBottom;++i, ++k)
		{
			for(unsigned int j=widthMinusRight*componentsCount, l=(imgWidget->getWidth()-right)*componentsCount;j<width*componentsCount;++j, ++l)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*componentsCount + l];
			}
		}

		//copy corner bottom right
		for(unsigned int i=heightMinusBottom, k=imgWidget->getHeight()-bottom;i<height;++i, ++k)
		{
			for(unsigned int j=widthMinusRight*componentsCount, l=(imgWidget->getWidth()-right)*componentsCount;j<width*componentsCount;++j, ++l)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[k*imgWidget->getWidth()*componentsCount + l];
			}
		}

		//copy bottom
		for(unsigned int i=heightMinusBottom, k=imgWidget->getHeight()-bottom;i<height;++i, ++k)
		{
			for(unsigned int j=left*componentsCount, l=0;j<widthMinusRight*componentsCount;++j, ++l)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[k*imgWidget->getWidth()*componentsCount + l%((imgWidget->getWidth()-(left+right))*componentsCount) + left*componentsCount];
			}
		}

		//copy corner bottom left
		for(unsigned int i=heightMinusBottom, k=0;i<height;++i, ++k)
		{
			for(unsigned int j=0;j<leftMultiplyInternalFormat;++j)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[(k + imgWidget->getHeight()-bottom)*imgWidget->getWidth()*componentsCount + j];
			}
		}

		//copy left
		for(unsigned int i=top, k=0;i<heightMinusBottom;++i, ++k)
		{
			for(unsigned int j=0;j<leftMultiplyInternalFormat;++j)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*componentsCount + j];
			}
		}

		//copy middle
		for(unsigned int i=top, k=0;i<heightMinusBottom;++i, ++k)
		{
			for(unsigned int j=leftMultiplyInternalFormat, l=0;j<widthMinusRight*componentsCount;++j, ++l)
			{
				texels[i*width*componentsCount + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*componentsCount + l%((imgWidget->getWidth()-(left+right))*componentsCount) + left*componentsCount];
			}
		}

		//create the texture
		std::shared_ptr<Image> texWidget = std::shared_ptr<Image>(new Image(width, height, imgWidget->getImageFormat(), std::move(texels)), Resource::ResourceDeleter());
		texWidget->toTexture(false, false, false);

		imgWidget->release();

		return texWidget;
	}

	XmlParser *GUISkinService::getXmlSkin() const
	{
		if(!xmlSkin)
		{
			throw std::runtime_error("Skin for the GUI isn't initialized.");
		}

		return xmlSkin;
	}

}
