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

	Image *GUISkinService::createTexWidget(int width, int height, std::shared_ptr<XmlChunk> skinXmlChunk, WidgetOutline *widgetOutline) const
	{
		//skin informations
		std::shared_ptr<XmlChunk> imgWidgetElem = getXmlSkin()->getUniqueChunk(true, "image", XmlAttribute(), skinXmlChunk);
		Image *imgWidget = MediaManager::instance()->getMedia<Image>(imgWidgetElem->getStringValue());

		std::shared_ptr<XmlChunk> topElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "top"), skinXmlChunk));
		int top = topElem->getIntValue();

		std::shared_ptr<XmlChunk> bottomElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "bottom"), skinXmlChunk));
		int bottom = bottomElem->getIntValue();

		std::shared_ptr<XmlChunk> leftElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "left"), skinXmlChunk));
		int left = leftElem->getIntValue();

		std::shared_ptr<XmlChunk> rightElem(getXmlSkin()->getUniqueChunk(true, "part", XmlAttribute("zone", "right"), skinXmlChunk));
		int right = rightElem->getIntValue();

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

		//copy corner top left
		for(int i=0;i<top;++i)
		{
			for(int j=0;j<left*internalFormat;++j)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*internalFormat + j];
			}
		}

		//copy top
		for(int i=0;i<top;++i)
		{
			for(int j=left*internalFormat, k=0;j<(width-right)*internalFormat;++j, ++k)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*internalFormat + k%((imgWidget->getWidth()-(right+left))*internalFormat) + left*internalFormat];
			}
		}

		//copy corner top right
		for(int i=0;i<top;++i)
		{
			for(int j=(width-right)*internalFormat, k=(imgWidget->getWidth()-right)*internalFormat;j<width*internalFormat;++j, ++k)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[i*imgWidget->getWidth()*internalFormat + k];
			}
		}

		//copy right
		for(int i=top, k=0;i<height-bottom;++i, ++k)
		{
			for(int j=(width-right)*internalFormat, l=(imgWidget->getWidth()-right)*internalFormat;j<width*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*internalFormat + l];
			}
		}

		//copy corner bottom right
		for(int i=height-bottom, k=imgWidget->getHeight()-bottom;i<height;++i, ++k)
		{
			for(int j=(width-right)*internalFormat, l=(imgWidget->getWidth()-right)*internalFormat;j<width*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[k*imgWidget->getWidth()*internalFormat + l];
			}
		}

		//copy bottom
		for(int i=height-bottom, k=imgWidget->getHeight()-bottom;i<height;++i, ++k)
		{
			for(int j=left*internalFormat, l=0;j<(width-right)*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[k*imgWidget->getWidth()*internalFormat + l%((imgWidget->getWidth()-(left+right))*internalFormat) + left*internalFormat];
			}
		}

		//copy corner bottom left
		for(int i=height-bottom, k=0;i<height;++i, ++k)
		{
			for(int j=0;j<left*internalFormat;++j)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k + imgWidget->getHeight()-bottom)*imgWidget->getWidth()*internalFormat + j];
			}
		}

		//copy left
		for(int i=top, k=0;i<height-bottom;++i, ++k)
		{
			for(int j=0;j<left*internalFormat;++j)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*internalFormat + j];
			}
		}

		//copy middle
		for(int i=top, k=0;i<height-bottom;++i, ++k)
		{
			for(int j=left*internalFormat, l=0;j<(width-right)*internalFormat;++j, ++l)
			{
				texels[i*width*internalFormat + j] = imgWidget->getTexels()[(k%(imgWidget->getHeight()-(top+bottom)) + top)*imgWidget->getWidth()*internalFormat + l%((imgWidget->getWidth()-(left+right))*internalFormat) + left*internalFormat];
			}
		}

		//create the texture
		Image *texWidget = new Image(imgWidget->getInternalFormat(), width, height, imgWidget->getFormat(), texels);
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
