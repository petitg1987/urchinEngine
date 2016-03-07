#include <GL/gl.h>

#include "scene/GUI/button/Button.h"
#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/text/Text.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	Button::Button(int positionX, int positionY, int width, int height, const std::string &nameSkin, const std::string &text)
		: Widget(positionX, positionY, width, height)
	{
		//skin informations
		std::shared_ptr<XmlChunk> buttonChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "button", XmlAttribute("nameSkin", nameSkin));

		std::shared_ptr<XmlChunk> skinDefaultChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), buttonChunk);
		texInfoDefault = GUISkinService::instance()->createTexWidget(width, height, skinDefaultChunk);

		std::shared_ptr<XmlChunk> skinFocusChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), buttonChunk);
		texInfoOnFocus = GUISkinService::instance()->createTexWidget(width, height, skinFocusChunk);

		std::shared_ptr<XmlChunk> skinClickChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "click"), buttonChunk);
		texInfoOnClick = GUISkinService::instance()->createTexWidget(width, height, skinClickChunk);

		if(!text.empty())
		{
			std::shared_ptr<XmlChunk> textChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), buttonChunk);
			Text *msg = new Text(0, 0, textChunk->getStringValue());
			addChild(msg);
			msg->setText(text.c_str());
			msg->setPosition((int)(width - msg->getWidth())/2, (int)(height - msg->getHeight())/2);
		}

		//visual
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_INT, new int[8]{0, 0, width, 0, width, height, 0, height})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, texInfoDefault->getMaxCoordS(), 0.0, texInfoDefault->getMaxCoordS(), texInfoDefault->getMaxCoordT(), 0.0, texInfoDefault->getMaxCoordT()})
				->build();

		textureID = texInfoDefault->getTextureID();
	}

	Button::~Button()
	{
		texInfoDefault->release();
		texInfoOnFocus->release();
		texInfoOnClick->release();
	}

	unsigned int Button::getTextureId()
	{
		if(getWidgetState()==FOCUS)
		{
			return texInfoOnFocus->getTextureID();
		}else if(getWidgetState()==CLICKING)
		{
			textureID = texInfoOnClick->getTextureID();
		}

		return texInfoDefault->getTextureID();
	}

	bool Button::onKeyDownEvent(unsigned int key)
	{
		textureID = getTextureId();
		return true;
	}

	bool Button::onKeyUpEvent(unsigned int key)
	{
		textureID = getTextureId();
		return true;
	}

	bool Button::onMouseMoveEvent(int mouseX, int mouseY)
	{
		textureID = getTextureId();
		return true;
	}

	void Button::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);

		quadDisplayer->display();

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}
	
}
