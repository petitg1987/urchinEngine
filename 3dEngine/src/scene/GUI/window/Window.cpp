#include <GL/gl.h>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/window/Window.h"
#include "scene/GUI/text/Text.h"
#include "scene/SceneManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

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
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_INT, new int[8]{0, 0, width, 0, width, height, 0, height})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, texWindow->getMaxCoordS(), 0.0, texWindow->getMaxCoordS(), texWindow->getMaxCoordT(), 0.0, texWindow->getMaxCoordT()})
				->build();
	}

	Window::~Window()
	{
		delete widgetOutline;
		texWindow->release();
	}

	void Window::addChild(Widget *child)
	{
		child->setPosition(child->getPositionX()+widgetOutline->leftWidth, child->getPositionY()+widgetOutline->topWidth);
		Widget::addChild(child);
	}

	bool Window::onKeyDown(unsigned int key)
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
		bool propagateEvent = true;
		if(key==KEY_MOUSE_LEFT && widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			notifyObservers(this, SET_IN_FOREGROUND);
			propagateEvent = false;
		}
		
		return Widget::onKeyDown(key) && propagateEvent;
	}

	bool Window::onKeyUp(unsigned int key)
	{
		Rectangle<int> closeZone(Point2<int>(getGlobalPositionX()+(width-widgetOutline->rightWidth), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+widgetOutline->topWidth));
		if(key==KEY_MOUSE_LEFT && state==CLOSING && closeZone.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			setIsVisible(false);
		}
		
		state = DEFAULT;

		return Widget::onKeyUp(key);
	}

	bool Window::onMouseMove(int mouseX, int mouseY)
	{
		if(state==MOVING)
		{
			setPosition(mouseX - mousePositionX, mouseY - mousePositionY);
		}
		
		return Widget::onMouseMove(mouseX, mouseY);
	}

	void Window::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, texWindow->getTextureID());

		quadDisplayer->display();

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
