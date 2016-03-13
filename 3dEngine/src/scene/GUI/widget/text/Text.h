#ifndef ENGINE_TEXT_H
#define ENGINE_TEXT_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "resources/font/Font.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{
	
	class Text : public Widget
	{
		public:
			Text(Position, const std::string &);
			virtual ~Text();
		
			void createOrUpdateWidget();

			void setText(const std::string &, int maxLength=-1);
			const Font *getFont();
			
			void display(int, float);

		private:
			std::string cutText(const std::string &, int);

			//properties
			std::string text;
			int maxLength;

			//visual
			Font *font;
			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};
	
}

#endif
