#include "NavMeshGenerator.h"

namespace urchin
{

	NavMeshGenerator::NavMeshGenerator(std::shared_ptr<AIWorld> aiWorld, NavMeshConfig navMeshConfig) :
		aiWorld(aiWorld),
		navMeshConfig(navMeshConfig)
	{

	}

	std::shared_ptr<NavMesh> NavMeshGenerator::generate()
	{
		std::shared_ptr<NavMesh> navMesh = std::make_shared<NavMesh>();

		std::cout<<"Generate Nav Mesh"<<std::endl;

		return navMesh;
	}

}
