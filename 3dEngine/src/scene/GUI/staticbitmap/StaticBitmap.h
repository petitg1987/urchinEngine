#ifndef ENGINE_STATICBITMAP_H
#define ENGINE_STATICBITMAP_H

#include <string>
#include <memory>

#include "scene/GUI/Widget.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class StaticBitmap : public Widget
	{
		public:
			StaticBitmap(int, int, Size, const std::string &);
			virtual ~StaticBitmap();
			
			void createOrUpdateWidget();

			void display(int, float);
			
		private:
			//properties
			const std::string filename;

			//visual
			Image *tex;
			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};
	
}

#endif
