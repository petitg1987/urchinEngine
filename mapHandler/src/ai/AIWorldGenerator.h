#ifndef ENGINE_AIWORLDGENERATOR_H
#define ENGINE_MAPNAVMESHGENERATOR_H

#include <memory>
#include "UrchinAIEngine.h"

#include "resources/Map.h"

namespace urchin
{

	class AIWorldGenerator
	{
		public:
			std::shared_ptr<AIWorld> generate(const Map *);

	};

}

#endif
