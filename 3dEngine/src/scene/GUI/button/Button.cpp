#include <GL/gl.h>

#include "scene/GUI/button/Button.h"
#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/text/Text.h"
#include "scene/SceneManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	Button::Button(int positionX, int positionY, int width, int height, const std::string &nameSkin, std::shared_ptr<ButtonCommand> buttonCommand, const std::string &text)
		: Widget(positionX, positionY, width, height),
		  buttonCommand(buttonCommand),
		  state(DEFAULT)
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
		quadDisplayer = std::make_shared<QuadDisplayerBuilder>()
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

	void Button::onKeyDown(unsigned int key)
	{
		if(key==KEY_MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+height));
			if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				state=CLICKING;
				textureID = texInfoOnClick->getTextureID();
			}
		}

		Widget::onKeyDown(key);
	}

	void Button::onKeyUp(unsigned int key)
	{
		if(key==KEY_MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+height));
			if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				if(state==CLICKING)
				{
					buttonCommand->onClick();
				}
				state = FOCUS;
				textureID = texInfoOnFocus->getTextureID();
			}
			else
			{
				state = DEFAULT;
				textureID = texInfoDefault->getTextureID();
			}
		}

		Widget::onKeyUp(key);
	}

	void Button::onMouseMove(int mouseX, int mouseY)
	{
		Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+height));
		if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			if(state==DEFAULT)
			{
				state = FOCUS;
				textureID = texInfoOnFocus->getTextureID();
			}
		}else if(state==FOCUS)
		{
			state = DEFAULT;
			textureID = texInfoDefault->getTextureID();
		}

		Widget::onMouseMove(mouseX, mouseY);
	}

	void Button::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);

		quadDisplayer->display();

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}
	
}
