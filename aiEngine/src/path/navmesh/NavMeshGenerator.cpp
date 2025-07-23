#include <string>

#include "path/navmesh/NavMeshGenerator.h"

namespace urchin {

    //debug parameters
    constexpr auto DEBUG_EXPORT_NAV_MESH = False();

    NavMeshGenerator::NavMeshGenerator() :
            navMeshAgent(std::make_unique<NavMeshAgent>()),
            navMesh(std::make_shared<NavMesh>()),
            needFullRefresh(false) {

    }

    void NavMeshGenerator::setNavMeshAgent(const NavMeshAgent& navMeshAgent) {
        std::scoped_lock lock(navMeshMutex);

        this->navMeshAgent = std::make_unique<NavMeshAgent>(navMeshAgent);
        this->needFullRefresh.store(true, std::memory_order_release);
    }

    const NavMeshAgent& NavMeshGenerator::getNavMeshAgent() const {
        return *navMeshAgent;
    }

    NavMesh NavMeshGenerator::copyLastGeneratedNavMesh() const {
        std::scoped_lock lock(navMeshMutex);

        return NavMesh(*navMesh);
    }

    /**
     * See '_doc' for an algorithm overview
     */
    std::shared_ptr<NavMesh> NavMeshGenerator::generate(AIWorld& /*aiWorld*/) {
        ScopeProfiler sp(Profiler::ai(), "navMeshGenerate");

        //TO DO: update 'allNavPolygons' / 'allNavObjects' (model to review ?) from 'aiWorld'
        updateNavMesh();

        if (DEBUG_EXPORT_NAV_MESH) {
            navMesh->svgMeshExport(SystemInfo::homeDirectory() + "navMesh/navMesh" + std::to_string(navMesh->getUpdateId()) + ".svg");
        }

        return navMesh;
    }

    void NavMeshGenerator::updateNavMesh() {
        allNavPolygons.clear();
        allNavObjects.clear();

        //TO DO: fill 'allNavPolygons' from 'allNavObjects' (model to review ?)

        std::scoped_lock lock(navMeshMutex);
        navMesh->copyAllPolygons(allNavPolygons);
    }

}
