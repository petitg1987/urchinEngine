#ifndef ENGINE_TEXT_H
#define ENGINE_TEXT_H

#include <string>

#include "scene/GUI/Widget.h"
#include "resources/font/Font.h"

namespace urchin
{
	
	class Text : public Widget
	{
		public:
			Text(int, int, const std::string &);
			virtual ~Text();
		
			void setText(const std::string &, int maxLength=-1);
			const Font *getFont();
			
			void display(int, float);

		private:
			std::string cutText(const std::string &, int);

			int numberLetter;

			//visual
			Font *font;

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
