#include <vector>

#include <collision/CollisionWorld.h>
#include <collision/OverlappingPair.h>

namespace urchin {

    CollisionWorld::CollisionWorld(BodyManager* bodyManager) :
            bodyManager(bodyManager),
            broadPhaseManager(std::make_unique<BroadPhaseManager>(bodyManager)),
            narrowPhaseManager(std::make_unique<NarrowPhaseManager>(bodyManager, getBroadPhaseManager())),
            integrateVelocityManager(std::make_unique<IntegrateVelocityManager>(bodyManager)),
            constraintSolverManager(std::make_unique<ConstraintSolverManager>()),
            islandManager(std::make_unique<IslandManager>(bodyManager)),
            integrateTransformManager(std::make_unique<IntegrateTransformManager>(bodyManager, getBroadPhaseManager(), getNarrowPhaseManager())) {

    }

    BroadPhaseManager& CollisionWorld::getBroadPhaseManager() const {
        return *broadPhaseManager;
    }

    NarrowPhaseManager& CollisionWorld::getNarrowPhaseManager() const {
        return *narrowPhaseManager;
    }

    /**
     * Update bodies by performing collision tests and responses
     * @param dt Delta of time (sec.) between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void CollisionWorld::process(float dt, const Vector3<float>& gravity) {
        ScopeProfiler sp(Profiler::physics(), "colWorldProc");

        //refresh bodies: add new bodies, remove bodies...
        bodyManager->refreshBodies();

        //broad phase: determine pairs of bodies potentially colliding based on their AABBox
        const std::vector<OverlappingPair*>& overlappingPairs = broadPhaseManager->computeOverlappingPairs();

        //integrate bodies velocities: gravity, external forces...
        integrateVelocityManager->integrateVelocity(dt, overlappingPairs, gravity);

        //narrow phase: check if pair of bodies colliding and update collision constraints
        manifoldResults.clear();
        narrowPhaseManager->process(dt, overlappingPairs, manifoldResults);
        notifyObservers(this, COLLISION_RESULT_UPDATED);

        //constraints solver: solve collision constraints
        constraintSolverManager->solveConstraints(dt, manifoldResults);

        //update bodies state
        islandManager->refreshBodyActiveState(manifoldResults);

        //integrate transformations
        integrateTransformManager->integrateTransform(dt);
    }

    const std::vector<ManifoldResult>& CollisionWorld::getLastUpdatedManifoldResults() {
        return manifoldResults;
    }

}
