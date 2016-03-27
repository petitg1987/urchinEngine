#ifndef ENGINE_TEXTBOX_H
#define ENGINE_TEXTBOX_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/GUISkinService.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{
	
	class TextBox : public Widget
	{
		public:
			TextBox(Position, Size, const std::string &);
			virtual ~TextBox();
			
			void createOrUpdateWidget();

			std::string getText();
		
			void display(int, float);
			
		private:
			bool onKeyDownEvent(unsigned int);
			bool onCharEvent(unsigned int);
			void reset();
		
			void refreshText(int);
			void computeCursorPosition();
			void computeCursorIndex(int);

			//properties
			const std::string nameSkin;

			//display informations
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

			//visual
			std::shared_ptr<Image> texTextBoxDefault, texTextBoxFocus;
			unsigned int textureID;
			WidgetOutline *widgetOutline;
			std::shared_ptr<QuadDisplayer> quadDisplayer;
			unsigned int cursorLineBufferID, cursorLineVAO;
			enum
			{
				SHADER_VERTEX_POSITION = 0,
			};
	};

}

#endif
