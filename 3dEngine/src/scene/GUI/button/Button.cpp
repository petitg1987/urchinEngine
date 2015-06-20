#include <GL/glew.h>
#include <GL/gl.h>

#include "scene/GUI/button/Button.h"
#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/text/Text.h"
#include "scene/SceneManager.h"

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
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		const int vertexArray[] = {0, 0, width, 0, width, height, 0, height};
		const float stArray[] = {0.0, 0.0, texInfoDefault->getMaxCoordS(), 0.0, texInfoDefault->getMaxCoordS(), texInfoDefault->getMaxCoordT(), 0.0, texInfoDefault->getMaxCoordT()};

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 2, GL_INT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), stArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, false, 0, 0);

		textureID = texInfoDefault->getTextureID();
	}

	Button::~Button()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(2, bufferIDs);

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

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 4);

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}
	
}
