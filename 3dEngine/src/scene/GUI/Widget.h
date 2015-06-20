#ifndef ENGINE_WIDGET_H
#define ENGINE_WIDGET_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{
	
	class Widget : public Observable
	{
		public:
			Widget(int, int, int, int);
			virtual ~Widget();

			enum NotificationType
			{
				ADD_CHILD_WIDGET, //A child widget has been added
				REMOVE_CHILD_WIDGET, //A child widget has been removed
				KEYBOARD_LOCKED, //Keyboard is locked by the widget
				KEYBOARD_UNLOCKED, //Keyboard is unlocked by the widget
				SET_IN_FOREGROUND //Widget should be set in the foreground
			};

			Widget *getLastUpdatedChildWidget();
		
			virtual void addChild(Widget *);
			virtual void removeChild(Widget *);

			void setParent(Widget *);
			Widget *getParent() const;
		
			void setPosition(int, int);
			int getPositionX() const;
			int getPositionY() const;
			int getGlobalPositionX() const;
			int getGlobalPositionY() const;

			int getWidth() const;
			int getHeight() const;
		
			void setIsVisible(bool);
			bool isVisible() const;

			void onPositionChange();
			const Vector2<int> &getTranslateDistance();
		
			virtual void onKeyDown(unsigned int);
			virtual void onKeyUp(unsigned int);
			virtual void onChar(unsigned int);
			virtual void onMouseMove(int, int);
			virtual void reset();

			virtual void display(int, float);

		private:
			void onChildWidgetEvent(Widget *const, NotificationType);

			Widget *parent;

			int positionX, positionY;
			bool bIsVisible;

		protected:
			std::vector<Widget *> children;
			Widget *lastUpdatedChildWidget;

			int width, height;
			Vector2<int> translateDistance;

			int mouseX, mouseY;
	};

}

#endif
