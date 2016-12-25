#ifndef ENGINE_AIWORLD_H
#define ENGINE_AIWORLD_H

#include <vector>

#include "input/AIObject.h"

namespace urchin
{

	class AIWorld
	{
		public:
			AIWorld();

			void addObject(const AIObject &);
			const std::vector<AIObject> &getObjects() const;

		private:
			std::vector<AIObject> objects;
	};

}

#endif
