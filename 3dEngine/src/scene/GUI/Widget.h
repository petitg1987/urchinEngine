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
				SET_IN_FOREGROUND //Widget should be set in the foreground
			};
		
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
		
			virtual bool onKeyDown(unsigned int);
			virtual bool onKeyUp(unsigned int);
			virtual bool onChar(unsigned int);
			virtual bool onMouseMove(int, int);
			virtual void reset();

			virtual void display(int, float);

		private:
			Widget *parent;

			int positionX, positionY;
			bool bIsVisible;

		protected:
			std::vector<Widget *> children;

			int width, height;
			Vector2<int> translateDistance;

			int mouseX, mouseY;
	};

}

#endif
