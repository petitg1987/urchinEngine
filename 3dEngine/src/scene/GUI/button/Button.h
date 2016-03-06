#ifndef ENGINE_BUTTON_H
#define ENGINE_BUTTON_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/Widget.h"
#include "scene/GUI/button/ButtonCommand.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class Button : public Widget
	{
		public:
			Button(int, int, int, int, const std::string &, std::shared_ptr<ButtonCommand>, const std::string &);
			virtual ~Button();
			
			void display(int, float);
			
		private:
			bool onKeyDown(unsigned int);
			bool onKeyUp(unsigned int);
			bool onMouseMove(int, int);
			
			std::shared_ptr<ButtonCommand> buttonCommand;

			//state
			enum buttonStates
			{
				DEFAULT,
				CLICKING,
				FOCUS
			} state;

			//visual
			unsigned int textureID;
			Image *texInfoDefault, *texInfoOnFocus, *texInfoOnClick;

			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
