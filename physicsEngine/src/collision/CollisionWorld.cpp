#include <vector>

#include <collision/CollisionWorld.h>
#include <collision/OverlappingPair.h>

namespace urchin {

    CollisionWorld::CollisionWorld(BodyContainer& bodyContainer) :
            bodyContainer(bodyContainer),
            broadPhase(BroadPhase(bodyContainer)),
            narrowPhase(NarrowPhase(bodyContainer, getBroadPhase())),
            integrateVelocity(IntegrateVelocity(bodyContainer)),
            constraintSolver(ConstraintSolver()),
            bodyActiveStateUpdater(BodyActiveStateUpdater(bodyContainer)),
            integrateTransform(IntegrateTransform(bodyContainer, getBroadPhase(), getNarrowPhase())) {

    }

    BroadPhase& CollisionWorld::getBroadPhase() {
        return broadPhase;
    }

    NarrowPhase& CollisionWorld::getNarrowPhase() {
        return narrowPhase;
    }

    /**
     * Update bodies by performing collision tests and responses
     * @param dt Delta of time (sec.) between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void CollisionWorld::process(float dt, const Vector3<float>& gravity) {
        ScopeProfiler sp(Profiler::physics(), "colWorldProc");

        //refresh bodies: add new bodies, remove bodies...
        bodyContainer.refreshBodies();

        //broad phase: determine pairs of bodies potentially colliding based on their AABBox
        auto& overlappingPairs = broadPhase.computeOverlappingPairs();

        //integrate bodies velocities: gravity, external forces...
        integrateVelocity.process(dt, overlappingPairs, gravity);

        //narrow phase: check if pair of bodies colliding and update collision constraints
        manifoldResults.clear();
        narrowPhase.process(dt, overlappingPairs, manifoldResults);
        notifyObservers(this, COLLISION_RESULT_UPDATED);

        //constraints solver: solve collision constraints
        constraintSolver.process(dt, manifoldResults);

        //update bodies state
        bodyActiveStateUpdater.update(manifoldResults);

        //integrate transformations
        integrateTransform.process(dt);
    }

    const std::vector<ManifoldResult>& CollisionWorld::getLastUpdatedManifoldResults() const {
        return manifoldResults;
    }

}
