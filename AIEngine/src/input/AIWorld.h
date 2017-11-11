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

			void addObject(const std::shared_ptr<AIObject> &);
            void removeObject(const std::shared_ptr<AIObject> &);

			std::vector<std::shared_ptr<AIObject>> getObjects() const;

		private:
			mutable std::mutex mutex;

			std::vector<std::shared_ptr<AIObject>> objects;
	};

}

#endif
