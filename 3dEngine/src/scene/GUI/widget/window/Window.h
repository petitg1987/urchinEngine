#ifndef URCHINENGINE_WINDOW_H
#define URCHINENGINE_WINDOW_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{
	
	class Window : public Widget
	{
		public:
			Window(Position, Size, const std::string &, const std::string &stringTitle = "");
			virtual ~Window();
		
			void createOrUpdateWidget();

			void addChild(Widget *);

			void display(int, float);
		
		private:
			bool onKeyDownEvent(unsigned int);
			bool onKeyUpEvent(unsigned int);
			bool onMouseMoveEvent(int, int);

			//properties
			const std::string nameSkin;
			const std::string stringTitle;

			//state
			int mousePositionX, mousePositionY;
			enum windowStates
			{
				DEFAULT,
				MOVING,
				CLOSING
			} state;
			
			//visual
			Text *title;
			std::shared_ptr<Image> texWindow;
			WidgetOutline *widgetOutline;
			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
