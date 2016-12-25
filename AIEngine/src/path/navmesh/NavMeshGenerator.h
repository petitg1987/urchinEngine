#ifndef ENGINE_NAVMESHGENERATOR_H
#define ENGINE_NAVMESHGENERATOR_H

#include <memory>

#include "input/AIWorld.h"
#include "path/navmesh/NavMeshConfig.h"
#include "path/navmesh/NavMesh.h"

namespace urchin
{

	class NavMeshGenerator
	{
		public:
			NavMeshGenerator(std::shared_ptr<AIWorld>, NavMeshConfig);

			std::shared_ptr<NavMesh> generate();

		private:
			std::shared_ptr<AIWorld> aiWorld;
			NavMeshConfig navMeshConfig;
	};

}

#endif
