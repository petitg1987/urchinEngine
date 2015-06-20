#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <string>

#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/Widget.h"
#include "resources/image/Image.h"

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
			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);
			void onMouseMove(int, int);

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

			unsigned int bufferIDs[2], vertexArrayObject;
			enum //buffer IDs indexes
			{
				VAO_VERTEX_POSITION = 0,
				VAO_TEX_COORD
			};
			enum //shader input
			{
				SHADER_VERTEX_POSITION = 0,
				SHADER_TEX_COORD
			};
	};

}

#endif
