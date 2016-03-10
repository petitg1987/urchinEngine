#include <GL/gl.h>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/window/Window.h"
#include "scene/SceneManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	Window::Window(int positionX, int positionY, Size size, const std::string &nameSkin, const std::string &stringTitle) :
		Widget(positionX, positionY, size),
		nameSkin(nameSkin),
		stringTitle(stringTitle),
		state(DEFAULT),
		title(nullptr),
		widgetOutline(new WidgetOutline())
	{
		createOrUpdateWidget();
	}

	Window::~Window()
	{
		delete widgetOutline;
	}

	void Window::createOrUpdateWidget()
	{
		//load the skin
		std::shared_ptr<XmlChunk> windowChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "window", XmlAttribute("nameSkin", nameSkin));

		//creates the texture
		std::shared_ptr<XmlChunk> skinChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute(), windowChunk);
		texWindow = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinChunk, widgetOutline);

		//creates font for title
		if(!stringTitle.empty())
		{
			std::shared_ptr<XmlChunk> textChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), windowChunk);
			Widget::removeChild(title);
			title = new Text(0, 0, textChunk->getStringValue());
			title->setPosition(widgetOutline->leftWidth, (widgetOutline->topWidth - title->getHeight())/2);
			title->setText(stringTitle.c_str());
			Widget::addChild(title);
		}

		//visual
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0})
				->build();
	}

	void Window::addChild(Widget *child)
	{
		child->setPosition(child->getPositionX()+widgetOutline->leftWidth, child->getPositionY()+widgetOutline->topWidth);
		Widget::addChild(child);
	}

	bool Window::onKeyDownEvent(unsigned int key)
	{
		Rectangle<int> titleZone(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+(getWidth()-widgetOutline->rightWidth), getGlobalPositionY()+widgetOutline->topWidth));
		Rectangle<int> closeZone(Point2<int>(getGlobalPositionX()+(getWidth() - widgetOutline->rightWidth), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+widgetOutline->topWidth));
		
		if(key==KEY_MOUSE_LEFT && titleZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY())))
		{
			mousePositionX = getMouseX() - getPositionX();
			mousePositionY = getMouseY() - getPositionY();
			
			state = MOVING;
		}else if(key==KEY_MOUSE_LEFT && closeZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY())))
		{
			state =CLOSING;
		}
		
		Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
		bool propagateEvent = true;
		if(key==KEY_MOUSE_LEFT && widgetRectangle.collideWithPoint(Point2<int>(getMouseX(), getMouseY())))
		{
			notifyObservers(this, SET_IN_FOREGROUND);
			propagateEvent = false;
		}
		
		return propagateEvent;
	}

	bool Window::onKeyUpEvent(unsigned int key)
	{
		Rectangle<int> closeZone(Point2<int>(getGlobalPositionX()+(getWidth()-widgetOutline->rightWidth), getGlobalPositionY()),
				Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+widgetOutline->topWidth));
		if(key==KEY_MOUSE_LEFT && state==CLOSING && closeZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY())))
		{
			setIsVisible(false);
		}
		
		state = DEFAULT;

		return true;
	}

	bool Window::onMouseMoveEvent(int mouseX, int mouseY)
	{
		if(state==MOVING)
		{
			setPosition(mouseX - mousePositionX, mouseY - mousePositionY);
		}
		
		return true;
	}

	void Window::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, texWindow->getTextureID());

		quadDisplayer->display();

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
