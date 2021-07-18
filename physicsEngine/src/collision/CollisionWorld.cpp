#include <vector>

#include <collision/CollisionWorld.h>
#include <collision/OverlappingPair.h>

namespace urchin {

    CollisionWorld::CollisionWorld(BodyManager& bodyManager) :
            bodyManager(bodyManager),
            broadPhase(std::make_unique<BroadPhase>(bodyManager)),
            narrowPhase(std::make_unique<NarrowPhase>(bodyManager, getBroadPhase())),
            integrateVelocity(std::make_unique<IntegrateVelocity>(bodyManager)),
            constraintSolver(std::make_unique<ConstraintSolver>()),
            bodyActiveStateUpdater(std::make_unique<BodyActiveStateUpdater>(bodyManager)),
            integrateTransform(std::make_unique<IntegrateTransform>(bodyManager, getBroadPhase(), getNarrowPhase())) {

    }

    BroadPhase& CollisionWorld::getBroadPhase() const {
        return *broadPhase;
    }

    NarrowPhase& CollisionWorld::getNarrowPhase() const {
        return *narrowPhase;
    }

    /**
     * Update bodies by performing collision tests and responses
     * @param dt Delta of time (sec.) between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void CollisionWorld::process(float dt, const Vector3<float>& gravity) {
        ScopeProfiler sp(Profiler::physics(), "colWorldProc");

        //refresh bodies: add new bodies, remove bodies...
        bodyManager.refreshBodies();

        //broad phase: determine pairs of bodies potentially colliding based on their AABBox
        auto& overlappingPairs = broadPhase->computeOverlappingPairs();

        //integrate bodies velocities: gravity, external forces...
        integrateVelocity->process(dt, overlappingPairs, gravity);

        //narrow phase: check if pair of bodies colliding and update collision constraints
        manifoldResults.clear();
        narrowPhase->process(dt, overlappingPairs, manifoldResults);
        notifyObservers(this, COLLISION_RESULT_UPDATED);

        //constraints solver: solve collision constraints
        constraintSolver->process(dt, manifoldResults);

        //update bodies state
        bodyActiveStateUpdater->update(manifoldResults);

        //integrate transformations
        integrateTransform->process(dt);
    }

    const std::vector<ManifoldResult>& CollisionWorld::getLastUpdatedManifoldResults() {
        return manifoldResults;
    }

}
