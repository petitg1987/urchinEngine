#ifndef ENGINE_BUTTON_H
#define ENGINE_BUTTON_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "scene/GUI/Widget.h"
#include "scene/GUI/button/ButtonCommand.h"
#include "resources/image/Image.h"

namespace urchin
{

	class Button : public Widget
	{
		public:
			Button(int, int, int, int, const std::string &, std::shared_ptr<ButtonCommand>, const std::string &);
			virtual ~Button();
			
			void display(int, float);
			
		private:
			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);
			void onMouseMove(int, int);
			
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
