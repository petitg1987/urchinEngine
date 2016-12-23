#ifndef ENGINE_INPUTWORLD_H
#define ENGINE_INPUTWORLD_H

#include <vector>

#include "path/input/InputObject.h"

namespace urchin
{

	class InputWorld
	{
		public:
			InputWorld();

			void addObject(const InputObject &);
			const std::vector<InputObject> &getObjects() const;

		private:
			std::vector<InputObject> objects;
	};

}

#endif
