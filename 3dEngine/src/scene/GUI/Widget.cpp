#include <GL/gl.h>

#include "scene/GUI/Widget.h"

namespace urchin
{

	Widget::Widget(int positionX, int positionY, int width, int height) :
		parent(nullptr), positionX(positionX), positionY(positionY), bIsVisible(true),
		width(width), height(height), mouseX(0), mouseY(0)
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

	void Widget::addChild(Widget *child)
	{
		child->setParent(this);
		children.push_back(child);
	}

	void Widget::removeChild(Widget *child)
	{
		std::vector<Widget *>::iterator it = std::find(children.begin(), children.end(), child);
		delete child;
		children.erase(it);
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

	int Widget::getWidth() const
	{
		return width;
	}

	int Widget::getHeight() const
	{
		return height;
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
		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				if(!children[i]->onKeyDown(key))
				{
					return false;
				}

			}
		}

		return true;
	}

	bool Widget::onKeyUp(unsigned int key)
	{
		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				if(!children[i]->onKeyUp(key))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool Widget::onChar(unsigned int character)
	{
		for(unsigned int i=0;i<children.size();++i)
		{
			if(children[i]->isVisible())
			{
				if(!children[i]->onChar(character)){
					return false;
				}
			}
		}
		return true;
	}

	bool Widget::onMouseMove(int mouseX, int mouseY)
	{
		this->mouseX = mouseX;
		this->mouseY = mouseY;

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
		return true;
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
