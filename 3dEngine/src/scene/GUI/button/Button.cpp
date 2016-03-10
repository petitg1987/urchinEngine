#include <GL/gl.h>

#include "scene/GUI/button/Button.h"
#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/text/Text.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	Button::Button(int positionX, int positionY, Size size, const std::string &nameSkin, const std::string &buttonText)
		: Widget(positionX, positionY, size),
		  nameSkin(nameSkin),
		  buttonText(buttonText)
	{
		createOrUpdateWidget();
	}

	Button::~Button()
	{

	}

	void Button::createOrUpdateWidget()
	{
		//skin informations
		std::shared_ptr<XmlChunk> buttonChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "button", XmlAttribute("nameSkin", nameSkin));

		std::shared_ptr<XmlChunk> skinDefaultChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), buttonChunk);
		texInfoDefault = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinDefaultChunk);

		std::shared_ptr<XmlChunk> skinFocusChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), buttonChunk);
		texInfoOnFocus = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinFocusChunk);

		std::shared_ptr<XmlChunk> skinClickChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "click"), buttonChunk);
		texInfoOnClick = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinClickChunk);

		if(!buttonText.empty())
		{
			std::shared_ptr<XmlChunk> textChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), buttonChunk);
			Text *msg = new Text(0, 0, textChunk->getStringValue());
			addChild(msg);
			msg->setText(buttonText.c_str());
			msg->setPosition((int)(getWidth() - msg->getWidth())/2, (int)(getHeight() - msg->getHeight())/2);
		}

		//visual
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0})
				->build();

		textureID = texInfoDefault->getTextureID();
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
