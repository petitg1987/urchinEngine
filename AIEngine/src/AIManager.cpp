#include <utility>
#include "UrchinCommon.h"

#include "AIManager.h"
#include "path/pathfinding/PathfindingAStar.h"

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
    }

    NavMeshGenerator *AIManager::getNavMeshGenerator() const
    {
        return navMeshGenerator;
    }

    std::vector<Point3<float>> AIManager::computePath(const Point3<float> &startPoint, const Point3<float> &endPoint) const
    {
        PathfindingAStar pathfindingAStar(navMeshGenerator->getNavMesh());
        return pathfindingAStar.findPath(startPoint, endPoint);
    }
}
