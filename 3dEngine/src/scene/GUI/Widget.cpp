#include <GL/gl.h>

#include "scene/GUI/Widget.h"
#include "scene/SceneManager.h"

namespace urchin
{

	Size::Size(float width, SizeType widthSizeType, float height, SizeType heightSizeType) :
			width(width), height(height), widthSizeType(widthSizeType), heightSizeType(heightSizeType)
	{

	}

	float Size::getWidth() const
	{
		return width;
	}

	Size::SizeType Size::getWidthSizeType() const
	{
		return widthSizeType;
	}

	float Size::getHeight() const
	{
		return height;
	}

	Size::SizeType Size::getHeightSizeType() const
	{
		return heightSizeType;
	}

	Widget::Widget(int positionX, int positionY, Size size) :
		sceneWidth(0),
		sceneHeight(0),
		parent(nullptr),
		widgetState(Widget::DEFAULT),
		size(size),
		positionX(positionX),
		positionY(positionY),
		bIsVisible(true),
		mouseX(0),
		mouseY(0)
	{
		onPositionChange();
	}

	Widget::~Widget()
	{
		for(unsigned int i=0;i<children.size();++i)
		{
			delete (children[i]);
		}
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

	void Widget::setParent(Widget *parent)
	{
		this->parent = parent;

		//if we set a parent, the position of the widget is affected by the parent
		onPositionChange();
	}

	Widget *Widget::getParent() const
	{
		return parent;
	}

	void Widget::setEventListener(const std::shared_ptr<EventListener> &eventListener)
	{
		this->eventListener = eventListener;
	}

	std::shared_ptr<EventListener> Widget::getEventListener() const
	{
		return eventListener;
	}

	Widget::WidgetStates Widget::getWidgetState() const
	{
		return widgetState;
	}

	void Widget::setPosition(int positionX, int positionY)
	{
		this->positionX = positionX;
		this->positionY = positionY;

		onPositionChange();
	}

	/**
	* @return Position X of the widget relative to his parent
	*/
	int Widget::getPositionX() const
	{
		return positionX;
	}

	/**
	* @return Position Y of the widget relative to his parent
	*/
	int Widget::getPositionY() const
	{
		return positionY;
	}

	int Widget::getGlobalPositionX() const
	{
		if(parent==nullptr)
		{
			return positionX;
		}

		return parent->getGlobalPositionX() + positionX;
	}

	int Widget::getGlobalPositionY() const
	{
		if(parent==nullptr)
		{
			return positionY;
		}

		return parent->getGlobalPositionY() + positionY;
	}

	void Widget::setWidth(unsigned int width, Size::SizeType sizeType)
	{
		this->size = Size(width, sizeType, size.getHeight(), size.getHeightSizeType());
	}

	unsigned int Widget::getWidth() const
	{
		if(size.getWidthSizeType()==Size::PERCENTAGE)
		{
			return size.getWidth() * sceneWidth;
		}
		return size.getWidth();
	}

	void Widget::setHeight(unsigned int height, Size::SizeType sizeType)
	{
		this->size = Size(size.getWidth(), size.getWidthSizeType(), height, sizeType);
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

	void Widget::onPositionChange()
	{
		//update model view matrix
		translateDistance.setValues(this->positionX, this->positionY);
		if(parent!=nullptr)
		{
			translateDistance += parent->getTranslateDistance();
		}

		//update model view matrix of children
		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				children[i]->onPositionChange();
			}
		}
	}

	const Vector2<int> &Widget::getTranslateDistance()
	{
		return translateDistance;
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
		if(key==KEY_MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
			if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				widgetState=CLICKING;
				if(eventListener)
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
		if(key==KEY_MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
			if(widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				if(eventListener && widgetState==CLICKING)
				{
					eventListener->onClickRelease(this);
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
				if(eventListener)
				{
					eventListener->onFocus(this);
				}
				widgetState = FOCUS;
			}
		}else if(widgetState==FOCUS)
		{
			if(eventListener)
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
				glUniform2iv(translateDistanceLoc, 1, (const int*)children[i]->getTranslateDistance());
				children[i]->display(translateDistanceLoc, invFrameRate);
			}
		}
	}

}
