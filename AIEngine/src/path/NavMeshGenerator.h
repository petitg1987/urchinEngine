#ifndef ENGINE_NAVMESHGENERATOR_H
#define ENGINE_NAVMESHGENERATOR_H

#include <memory>

#include "path/input/InputWorld.h"
#include "path/input/NavMeshConfig.h"

namespace urchin
{

	class NavMeshGenerator
	{
		public:
			NavMeshGenerator(std::shared_ptr<InputWorld> inputWorld, NavMeshConfig navMeshConfig);

			void generate();

		private:
			std::shared_ptr<InputWorld> inputWorld;
			NavMeshConfig navMeshConfig;
	};

}

#endif
