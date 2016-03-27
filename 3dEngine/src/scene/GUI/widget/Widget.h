#ifndef ENGINE_WIDGET_H
#define ENGINE_WIDGET_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/EventListener.h"

namespace urchin
{
	
	class Widget : public Observable
	{
		public:
			Widget(Position, Size);
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
		
			void onResize(unsigned int, unsigned int);
			virtual void createOrUpdateWidget() = 0;

			virtual void addChild(Widget *);
			virtual void removeChild(Widget *);

			void setParent(Widget *);
			Widget *getParent() const;

			void addEventListener(const std::shared_ptr<EventListener> &);
			const std::vector<std::shared_ptr<EventListener>> &getEventListeners() const;
			WidgetStates getWidgetState() const;
		
			void setPosition(Position);
			Position getPosition() const;
			int getPositionX() const;
			int getPositionY() const;
			int getGlobalPositionX() const;
			int getGlobalPositionY() const;

			void setSize(Size);
			Size getSize() const;
			unsigned int getWidth() const;
			unsigned int getHeight() const;
		
			void setIsVisible(bool);
			bool isVisible() const;

			bool onKeyDown(unsigned int);
			virtual bool onKeyDownEvent(unsigned int);
			bool onKeyUp(unsigned int);
			virtual bool onKeyUpEvent(unsigned int);
			bool onChar(unsigned int);
			virtual bool onCharEvent(unsigned int);
			bool onMouseMove(int, int);
			virtual bool onMouseMoveEvent(int, int);
			int getMouseX() const;
			int getMouseY() const;
			virtual void reset();

			virtual void display(int, float);

		protected:
			unsigned int getSceneWidth() const;
			unsigned int getSceneHeight() const;

			const std::vector<Widget *> &getChildren() const;

		private:
			void handleWidgetKeyDown(unsigned int);
			void handkeWidgetKeyUp(unsigned int);
			void handleWidgetMouseMove(int, int);

			unsigned int sceneWidth, sceneHeight;

			Widget *parent;
			std::vector<Widget *> children;

			std::vector<std::shared_ptr<EventListener>> eventListeners;
			WidgetStates widgetState;

			Position position;
			Size size;
			bool bIsVisible;

			int mouseX, mouseY;
	};

}

#endif
