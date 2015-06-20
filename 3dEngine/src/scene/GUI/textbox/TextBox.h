#ifndef ENGINE_TEXTBOX_H
#define ENGINE_TEXTBOX_H

#include <string>

#include "scene/GUI/Widget.h"
#include "scene/GUI/GUISkinService.h"
#include "scene/GUI/text/Text.h"

namespace urchin
{
	
	class TextBox : public Widget
	{
		public:
			TextBox(int, int, int, int, const std::string &);
			virtual ~TextBox();
			
			std::string getText();
		
			void display(int, float);
			
		private:
			void onKeyDown(unsigned int);
			void onChar(unsigned int);
			void reset();
		
			//display informations
			void refreshText(int);
			void computeCursorPosition();
			void computeCursorIndex(int);
			Text *text; //text of the text box (widget)
			std::string allText; //text of the text box (string)
			int maxWidthText; //maximum length of text can be displayed
			int startTextIndex; //index of the first letter to display
			int cursorIndex; //index of the cursor
			int cursorPosition; //position (in pixel) of the cursor

			//state
			enum textboxStates
			{
				ACTIVE,
				UNACTIVE
			} state;
			
			//cursor
			float cursorBlinkSpeed;

			//visual
			Image *texTextBoxDefault, *texTextBoxFocus;
			unsigned int textureID;
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
