#include <vector>

#include "collision/CollisionWorld.h"
#include "collision/OverlappingPair.h"

namespace urchin {

    CollisionWorld::CollisionWorld(BodyManager* bodyManager) :
            bodyManager(bodyManager),
            broadPhaseManager(new BroadPhaseManager(bodyManager)),
            narrowPhaseManager(new NarrowPhaseManager(bodyManager, broadPhaseManager)),
            integrateVelocityManager(new IntegrateVelocityManager(bodyManager)),
            constraintSolverManager(new ConstraintSolverManager()),
            islandManager(new IslandManager(bodyManager)),
            integrateTransformManager(new IntegrateTransformManager(bodyManager, broadPhaseManager, narrowPhaseManager)) {

    }

    CollisionWorld::~CollisionWorld() {
        delete broadPhaseManager;
        delete narrowPhaseManager;
        delete integrateVelocityManager;
        delete constraintSolverManager;
        delete islandManager;
        delete integrateTransformManager;
    }

    BroadPhaseManager* CollisionWorld::getBroadPhaseManager() const {
        return broadPhaseManager;
    }

    NarrowPhaseManager* CollisionWorld::getNarrowPhaseManager() const {
        return narrowPhaseManager;
    }

    /**
     * Update bodies by performing collision tests and responses
     * @param dt Delta of time (sec.) between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void CollisionWorld::process(float dt, const Vector3<float>& gravity) {
        ScopeProfiler profiler("physics", "colWorldProc");

        //initialize work bodies from bodies
        bodyManager->setupWorkBodies();

        //broad phase: determine pairs of bodies potentially colliding based on their AABBox
        const std::vector<OverlappingPair*>& overlappingPairs = broadPhaseManager->computeOverlappingPairs();

        //integrate bodies velocities (gravity, external forces...)
        integrateVelocityManager->integrateVelocity(dt, overlappingPairs, gravity);

        //narrow phase: check if pair of bodies colliding and update collision constraints
        manifoldResults.clear();
        narrowPhaseManager->process(dt, overlappingPairs, manifoldResults);
        notifyObservers(this, COLLISION_RESULT_UPDATED);

        //constraints solver: solve collision constraints
        constraintSolverManager->solveConstraints(dt, manifoldResults);

        //update bodies state and integrate transformations
        islandManager->refreshBodyActiveState(manifoldResults);
        integrateTransformManager->integrateTransform(dt);

        //apply work bodies to bodies
        bodyManager->applyWorkBodies();
    }

    const std::vector<ManifoldResult>& CollisionWorld::getLastUpdatedManifoldResults() {
        return manifoldResults;
    }

}
