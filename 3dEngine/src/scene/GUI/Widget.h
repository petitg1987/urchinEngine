#ifndef ENGINE_WIDGET_H
#define ENGINE_WIDGET_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/EventListener.h"

namespace urchin
{
	
	class Widget : public Observable
	{
		public:
			Widget(int, int, int, int);
			virtual ~Widget();

			enum NotificationType
			{
				SET_IN_FOREGROUND //Widget should be set in the foreground
			};

			enum WidgetStates
			{
				DEFAULT,
				CLICKING,
				FOCUS
			};
		
			virtual void addChild(Widget *);
			virtual void removeChild(Widget *);

			void setParent(Widget *);
			Widget *getParent() const;

			void setEventListener(std::shared_ptr<EventListener>);
			std::shared_ptr<EventListener> getEventListener() const;
			WidgetStates getWidgetState() const;
		
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
		
			bool onKeyDown(unsigned int);
			virtual bool onKeyDownEvent(unsigned int);
			bool onKeyUp(unsigned int);
			virtual bool onKeyUpEvent(unsigned int);
			bool onChar(unsigned int);
			virtual bool onCharEvent(unsigned int);
			bool onMouseMove(int, int);
			virtual bool onMouseMoveEvent(int, int);
			virtual void reset();

			virtual void display(int, float);

		private:
			void handleWidgetKeyDown(unsigned int);
			void handkeWidgetKeyUp(unsigned int);
			void handleWidgetMouseMove(int, int);

			Widget *parent;
			std::vector<Widget *> children;

			std::shared_ptr<EventListener> eventListener;
			WidgetStates widgetState;

			int positionX, positionY;
			Vector2<int> translateDistance;
			bool bIsVisible;

		protected:
			int width, height;
			int mouseX, mouseY;
	};

}

#endif
