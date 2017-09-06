#include <utility>
#include "UrchinCommon.h"

#include "AIManager.h"
#include "path/navmesh/NavMeshGenerator.h"
#include "path/pathfinding/AStar.h"

namespace urchin
{

    AIManager::AIManager() :
        navMeshGenerator(new NavMeshGenerator())
    {
        NumericalCheck::instance()->perform();
    }

    AIManager::~AIManager()
    {
        delete navMeshGenerator;
    }

    void AIManager::updateAI(std::shared_ptr<AIWorld> aiWorld)
    {
        navMeshGenerator->generate(std::move(aiWorld));

        AStar aStar(navMeshGenerator->getNavMesh());
        //TODO complete...
    }

    NavMeshGenerator *AIManager::getNavMeshGenerator() const
    {
        return navMeshGenerator;
    }
}
