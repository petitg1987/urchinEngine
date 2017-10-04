#ifndef URCHINENGINE_AIWORLD_H
#define URCHINENGINE_AIWORLD_H

#include <vector>

#include "input/AIObject.h"

namespace urchin
{

	class AIWorld
	{
		public:
            AIWorld();
			AIWorld(const std::shared_ptr<AIWorld> &);

			void addObject(const AIObject &);
			const std::vector<AIObject> &getObjects() const;

		private:
			std::vector<AIObject> objects;
	};

}

#endif
