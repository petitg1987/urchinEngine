#ifndef ENGINE_BUTTON_H
#define ENGINE_BUTTON_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class Button : public Widget
	{
		public:
			Button(Position, Size, const std::string &, const std::string &);
			virtual ~Button();
			
			void createOrUpdateWidget();

			void display(int, float);
			
		private:
			unsigned int getTextureId();

			bool onKeyDownEvent(unsigned int);
			bool onKeyUpEvent(unsigned int);
			bool onMouseMoveEvent(int, int);

			//properties
			const std::string nameSkin;
			const std::string buttonText;

			//visual
			unsigned int textureID;
			std::shared_ptr<Image> texInfoDefault, texInfoOnFocus, texInfoOnClick;
			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
