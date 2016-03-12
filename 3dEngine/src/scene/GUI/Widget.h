#ifndef ENGINE_WIDGET_H
#define ENGINE_WIDGET_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/EventListener.h"

namespace urchin
{
	
	class Size
	{
		public:
			enum SizeType{PIXEL, PERCENTAGE};
			Size(float width, SizeType widthSizeType, float height, SizeType heightSizeType);

			float getWidth() const;
			SizeType getWidthSizeType() const;

			float getHeight() const;
			SizeType getHeightSizeType() const;

		private:
			float width, height;
			SizeType widthSizeType, heightSizeType;
	};

	class Widget : public Observable
	{
		public:
			Widget(int, int, Size);
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

			void setEventListener(const std::shared_ptr<EventListener> &);
			std::shared_ptr<EventListener> getEventListener() const;
			WidgetStates getWidgetState() const;
		
			void setPosition(int, int);
			int getPositionX() const;
			int getPositionY() const;
			int getGlobalPositionX() const;
			int getGlobalPositionY() const;

			unsigned int getWidth() const;
			unsigned int getHeight() const;
		
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
			int getMouseX() const;
			int getMouseY() const;
			virtual void reset();

			virtual void display(int, float);

		protected:
			void setWidth(unsigned int, Size::SizeType);
			void setHeight(unsigned int, Size::SizeType);

		private:
			void handleWidgetKeyDown(unsigned int);
			void handkeWidgetKeyUp(unsigned int);
			void handleWidgetMouseMove(int, int);

			unsigned int sceneWidth, sceneHeight;

			Widget *parent;
			std::vector<Widget *> children;

			std::shared_ptr<EventListener> eventListener;
			WidgetStates widgetState;

			Size size;
			int positionX, positionY;
			Vector2<int> translateDistance;
			bool bIsVisible;

			int mouseX, mouseY;
	};

}

#endif
