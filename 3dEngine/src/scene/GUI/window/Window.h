#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <string>
#include <memory>

#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/Widget.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{
	
	class Window : public Widget
	{
		public:
			Window(int, int, int, int, const std::string &, const std::string &stringTitle = "");
			virtual ~Window();
		
			void addChild(Widget *);

			void display(int, float);
		
		private:
			bool onKeyDownEvent(unsigned int);
			bool onKeyUpEvent(unsigned int);
			bool onMouseMoveEvent(int, int);

			int mousePositionX, mousePositionY;

			//state
			enum windowStates
			{
				DEFAULT,
				MOVING,
				CLOSING
			} state;
			
			//visual
			Image *texWindow;
			WidgetOutline *widgetOutline;

			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
