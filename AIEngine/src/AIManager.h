#ifndef URCHINENGINE_AIMANAGER_H
#define URCHINENGINE_AIMANAGER_H

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

        private:
            NavMeshGenerator *navMeshGenerator;
    };

}

#endif
