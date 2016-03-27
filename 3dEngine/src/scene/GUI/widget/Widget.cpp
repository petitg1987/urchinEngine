#include <GL/gl.h>

#include "scene/GUI/widget/Widget.h"
#include "scene/InputDevice.h"

namespace urchin
{

	Widget::Widget(Position position, Size size) :
		sceneWidth(0),
		sceneHeight(0),
		parent(nullptr),
		widgetState(Widget::DEFAULT),
		position(position),
		size(size),
		bIsVisible(true),
		mouseX(0),
		mouseY(0)
	{

	}

	Widget::~Widget()
	{
		for(unsigned int i=0;i<children.size();++i)
		{
			delete (children[i]);
		}

		eventListeners.clear();
	}

	void Widget::onResize(unsigned int sceneWidth, unsigned int sceneHeight)
	{
		this->sceneWidth = sceneWidth;
		this->sceneHeight = sceneHeight;

		createOrUpdateWidget();

		for(unsigned int i=0;i<children.size();++i)
		{
			children[i]->onResize(sceneWidth, sceneHeight);
		}
	}

	unsigned int Widget::getSceneWidth() const
	{
		return sceneWidth;
	}

	unsigned int Widget::getSceneHeight() const
	{
		return sceneHeight;
	}

	void Widget::addChild(Widget *child)
	{
		child->setParent(this);
		children.push_back(child);
	}

	void Widget::removeChild(Widget *child)
	{
		if(child!=nullptr)
		{
			std::vector<Widget *>::iterator it = std::find(children.begin(), children.end(), child);
			delete child;
			children.erase(it);
		}
	}

	const std::vector<Widget *> &Widget::getChildren() const
	{
		return children;
	}

	void Widget::setParent(Widget *parent)
	{
		this->parent = parent;
	}

	Widget *Widget::getParent() const
	{
		return parent;
	}

	void Widget::addEventListener(const std::shared_ptr<EventListener> &eventListener)
	{
		this->eventListeners.push_back(eventListener);
	}

	const std::vector<std::shared_ptr<EventListener>> &Widget::getEventListeners() const
	{
		return eventListeners;
	}

	Widget::WidgetStates Widget::getWidgetState() const
	{
		return widgetState;
	}

	void Widget::setPosition(Position position)
	{
		this->position = position;
	}

	Position Widget::getPosition() const
	{
		return position;
	}

	/**
	* @return Position X of the widget relative to his parent
	*/
	int Widget::getPositionX() const
	{
		if(position.getPositionTypeX()==Position::PERCENTAGE)
		{
			return position.getPositionX() * sceneWidth;
		}

		return position.getPositionX();
	}

	/**
	* @return Position Y of the widget relative to his parent
	*/
	int Widget::getPositionY() const
	{
		if(position.getPositionTypeY()==Position::PERCENTAGE)
		{
			return position.getPositionY() * sceneHeight;
		}

		return position.getPositionY();
	}

	int Widget::getGlobalPositionX() const
	{
		if(parent==nullptr)
		{
			return getPositionX();
		}

		return parent->getGlobalPositionX() + getPositionX();
	}

	int Widget::getGlobalPositionY() const
	{
		if(parent==nullptr)
		{
			return getPositionY();
		}

		return parent->getGlobalPositionY() + getPositionY();
	}

	void Widget::setSize(Size size)
	{
		this->size = size;
	}

	Size Widget::getSize() const
	{
		return size;
	}

	unsigned int Widget::getWidth() const
	{
		if(size.getWidthSizeType()==Size::PERCENTAGE)
		{
			return size.getWidth() * sceneWidth;
		}
		return size.getWidth();
	}

	unsigned int Widget::getHeight() const
	{
		if(size.getHeightSizeType()==Size::PERCENTAGE)
		{
			return size.getHeight() * sceneHeight;
		}
		return size.getHeight();
	}

	void Widget::setIsVisible(bool isVisible)
	{
		this->bIsVisible = isVisible;
	}

	bool Widget::isVisible() const
	{
		return bIsVisible;
	}

	bool Widget::onKeyDown(unsigned int key)
	{
		handleWidgetKeyDown(key);

		bool propagateEvent = onKeyDownEvent(key);

		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible() && !children[i]->onKeyDown(key))
			{
				return false;
			}
		}

		return propagateEvent;
	}

	bool Widget::onKeyDownEvent(unsigned int key)
	{
		return true;
	}

	void Widget::handleWidgetKeyDown(unsigned int key)
	{
		if(key==InputDevice::Key::MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
			if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				widgetState=CLICKING;
				for(std::shared_ptr<EventListener> &eventListener : eventListeners)
				{
					eventListener->onClick(this);
				}
			}
		}
	}

	bool Widget::onKeyUp(unsigned int key)
	{
		handkeWidgetKeyUp(key);

		bool propagateEvent = onKeyUpEvent(key);

		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible() && !children[i]->onKeyUp(key))
			{
				return false;
			}
		}
		return propagateEvent;
	}

	bool Widget::onKeyUpEvent(unsigned int key)
	{
		return true;
	}

	void Widget::handkeWidgetKeyUp(unsigned int key)
	{
		if(key==InputDevice::Key::MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
			if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				if(widgetState==CLICKING)
				{
					for(std::shared_ptr<EventListener> &eventListener : eventListeners)
					{
						eventListener->onClickRelease(this);
					}
				}
				widgetState = FOCUS;
			}else
			{
				widgetState = DEFAULT;
			}
		}
	}

	bool Widget::onChar(unsigned int character)
	{
		if(!onCharEvent(character))
		{
			return false;
		}

		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible() && !children[i]->onChar(character)){
				return false;
			}
		}
		return true;
	}

	bool Widget::onCharEvent(unsigned int character)
	{
		return true;
	}

	bool Widget::onMouseMove(int mouseX, int mouseY)
	{
		this->mouseX = mouseX;
		this->mouseY = mouseY;

		handleWidgetMouseMove(mouseX, mouseY);

		bool propagateEvent = onMouseMoveEvent(mouseX, mouseY);

		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				if(!children[i]->onMouseMove(mouseX, mouseY))
				{
					return false;
				}
			}
		}
		return propagateEvent;
	}

	bool Widget::onMouseMoveEvent(int mouseX, int mouseY)
	{
		return true;
	}

	void Widget::handleWidgetMouseMove(int mouseX, int mouseY)
	{
		Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
		if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
		{
			if(widgetState==DEFAULT)
			{
				for(std::shared_ptr<EventListener> &eventListener : eventListeners)
				{
					eventListener->onFocus(this);
				}
				widgetState = FOCUS;
			}
		}else if(widgetState==FOCUS)
		{
			for(std::shared_ptr<EventListener> &eventListener : eventListeners)
			{
				eventListener->onFocusLost(this);
			}
			widgetState = DEFAULT;
		}
	}

	int Widget::getMouseX() const
	{
		return mouseX;
	}

	int Widget::getMouseY() const
	{
		return mouseY;
	}

	void Widget::reset()
	{
		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				children[i]->reset();
			}
		}
	}

	void Widget::display(int translateDistanceLoc, float invFrameRate)
	{
		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				Vector2<int> translateVector(children[i]->getGlobalPositionX(), children[i]->getGlobalPositionY());
				glUniform2iv(translateDistanceLoc, 1, (const int*)translateVector);

				children[i]->display(translateDistanceLoc, invFrameRate);
			}
		}
	}

}
