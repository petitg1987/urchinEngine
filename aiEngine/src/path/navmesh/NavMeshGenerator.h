#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include <input/AIWorld.h>
#include <path/navmesh/model/NavObject.h>
#include <path/navmesh/model/output/NavMeshAgent.h>
#include <path/navmesh/model/output/NavMesh.h>
#include <path/navmesh/model/output/NavPolygon.h>

namespace urchin {

    class NavMeshGenerator {
        public:
            NavMeshGenerator();

            void setNavMeshAgent(const NavMeshAgent&);
            const NavMeshAgent& getNavMeshAgent() const;

            std::shared_ptr<NavMesh> generate(AIWorld&);
            NavMesh copyLastGeneratedNavMesh() const;

        private:
            void updateNavMesh();

            mutable std::mutex navMeshMutex;
            std::unique_ptr<NavMeshAgent> navMeshAgent;
            std::shared_ptr<NavMesh> navMesh;
            std::atomic_bool needFullRefresh;

            std::vector<std::shared_ptr<NavObject>> allNavObjects;
            std::vector<std::shared_ptr<NavPolygon>> allNavPolygons;
    };

}
