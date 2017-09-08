#ifndef URCHINENGINE_AIMANAGER_H
#define URCHINENGINE_AIMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "input/AIWorld.h"
#include "path/navmesh/NavMeshGenerator.h"
#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavMeshConfig.h"

namespace urchin
{

    class AIManager
    {
        public:
            AIManager();
            ~AIManager();

            void updateAI(std::shared_ptr<AIWorld>);

            NavMeshGenerator *getNavMeshGenerator() const;
            std::vector<Point3<float>> computePath(const Point3<float> &, const Point3<float> &) const;

        private:
            NavMeshGenerator *navMeshGenerator;
    };

}

#endif
