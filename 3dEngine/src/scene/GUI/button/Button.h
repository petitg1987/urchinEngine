#ifndef ENGINE_BUTTON_H
#define ENGINE_BUTTON_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/Widget.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class Button : public Widget
	{
		public:
			Button(int, int, int, int, const std::string &, const std::string &);
			virtual ~Button();
			
			void display(int, float);
			
		private:
			unsigned int getTextureId();

			bool onKeyDownEvent(unsigned int);
			bool onKeyUpEvent(unsigned int);
			bool onMouseMoveEvent(int, int);

			//visual
			unsigned int textureID;
			Image *texInfoDefault, *texInfoOnFocus, *texInfoOnClick;

			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
