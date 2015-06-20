#ifndef ENGINE_STATICBITMAP_H
#define ENGINE_STATICBITMAP_H

#include <string>

#include "scene/GUI/Widget.h"
#include "resources/image/Image.h"

namespace urchin
{

	class StaticBitmap : public Widget
	{
		public:
			StaticBitmap(int, int, const std::string &);
			virtual ~StaticBitmap();
			
			void display(int, float);
			
		private:
			//visual
			Image *tex;

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
