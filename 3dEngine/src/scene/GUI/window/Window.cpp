#include <GL/gl.h>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/window/Window.h"
#include "scene/GUI/text/Text.h"
#include "scene/SceneManager.h"

namespace urchin
{

	Window::Window(int positionX, int positionY, int width, int height, const std::string &nameSkin, const std::string &stringTitle) :
		Widget(positionX, positionY, width, height), state(DEFAULT), widgetOutline(new WidgetOutline())
	{
		//load the skin
		std::shared_ptr<XmlChunk> windowChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "window", XmlAttribute("nameSkin", nameSkin));
		
		//creates the texture
		std::shared_ptr<XmlChunk> skinChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute(), windowChunk);
		texWindow = GUISkinService::instance()->createTexWidget(width, height, skinChunk, widgetOutline);

		//creates font for title
		if(!stringTitle.empty())
		{
			std::shared_ptr<XmlChunk> textChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), windowChunk);
			Text *title = new Text(0, 0, textChunk->getStringValue());
			addChild(title);
			title->setPosition(widgetOutline->leftWidth, (widgetOutline->topWidth - title->getHeight())/2);
			title->setText(stringTitle.c_str(), 300.0);
		}

		//visual
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		const int vertexArray[] = {0, 0, width, 0, width, height, 0, height};
		const float stArray[] = {0.0, 0.0, texWindow->getMaxCoordS(), 0.0, texWindow->getMaxCoordS(), texWindow->getMaxCoordT(), 0.0, texWindow->getMaxCoordT()};

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(VAO_VERTEX_POSITION);
		glVertexAttribPointer(VAO_VERTEX_POSITION, 2, GL_INT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), stArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, false, 0, 0);
	}

	Window::~Window()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(2, bufferIDs);

		delete widgetOutline;
		texWindow->release();
	}

	void Window::addChild(Widget *child)
	{
		child->setPosition(child->getPositionX()+widgetOutline->leftWidth, child->getPositionY()+widgetOutline->topWidth);
		Widget::addChild(child);
	}

	void Window::onKeyDown(unsigned int key)
	{
		Rectangle<int> titleZone(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+(width-widgetOutline->rightWidth), getGlobalPositionY()+widgetOutline->topWidth));
		Rectangle<int> closeZone(Point2<int>(getGlobalPositionX()+(width - widgetOutline->rightWidth), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+widgetOutline->topWidth));
		
		if(key==KEY_MOUSE_LEFT && titleZone.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			mousePositionX = mouseX - getPositionX();
			mousePositionY = mouseY - getPositionY();
			
			state = MOVING;
		}else if(key==KEY_MOUSE_LEFT && closeZone.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			state =CLOSING;
		}
		
		Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+height));
		if(key==KEY_MOUSE_LEFT && widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			notifyObservers(this, SET_IN_FOREGROUND);
		}
		
		Widget::onKeyDown(key);
	}

	void Window::onKeyUp(unsigned int key)
	{
		Rectangle<int> closeZone(Point2<int>(getGlobalPositionX()+(width-widgetOutline->rightWidth), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+widgetOutline->topWidth));
		if(key==KEY_MOUSE_LEFT && state==CLOSING && closeZone.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			setIsVisible(false);
		}
		
		state = DEFAULT;

		Widget::onKeyUp(key);
	}

	void Window::onMouseMove(int mouseX, int mouseY)
	{
		if(state==MOVING)
		{
			setPosition(mouseX - mousePositionX, mouseY - mousePositionY);
		}
		
		Widget::onMouseMove(mouseX, mouseY);
	}

	void Window::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, texWindow->getTextureID());

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 4);

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
