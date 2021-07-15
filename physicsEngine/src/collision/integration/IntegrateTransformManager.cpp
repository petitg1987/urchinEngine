#include <collision/integration/IntegrateTransformManager.h>
#include <shape/CollisionSphereShape.h>
#include <object/TemporalObject.h>

namespace urchin {

    //static
    const float IntegrateTransformManager::MAX_LINEAR_VELOCITY_FACTOR = 0.95f;

    IntegrateTransformManager::IntegrateTransformManager(const BodyManager& bodyManager, const BroadPhaseManager& broadPhaseManager,
                                                         const NarrowPhaseManager& narrowPhaseManager) :
            bodyManager(bodyManager),
            broadPhaseManager(broadPhaseManager),
            narrowPhaseManager(narrowPhaseManager) {

    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void IntegrateTransformManager::integrateTransform(float dt) {
        for (const auto& abstractBody : bodyManager.getBodies()) {
            RigidBody* body = RigidBody::upCast(abstractBody.get());
            if (body && body->isActive()) {
                PhysicsTransform currentTransform = body->getTransform();
                PhysicsTransform newTransform = currentTransform.integrate(body->getLinearVelocity(), body->getAngularVelocity(), dt);

                float ccdMotionThreshold = body->getCcdMotionThreshold();
                float motion = currentTransform.getPosition().vector(newTransform.getPosition()).length();

                if (motion > ccdMotionThreshold) {
                    handleContinuousCollision(*body, currentTransform, newTransform, dt);
                } else {
                    body->setTransform(newTransform);
                }
            }
        }
    }

    void IntegrateTransformManager::handleContinuousCollision(RigidBody& body, const PhysicsTransform& from, const PhysicsTransform& to, float dt) {
        PhysicsTransform updatedTargetTransform = to;

        std::vector<AbstractBody*> bodiesAABBoxHitBody = broadPhaseManager.bodyTest(body, from, to);
        if (!bodiesAABBoxHitBody.empty()) {
            CollisionSphereShape bodyEncompassedSphereShape(body.getShape().getMinDistanceToCenter());
            TemporalObject temporalObject(bodyEncompassedSphereShape, from, to);
            ccd_set ccdResults = narrowPhaseManager.continuousCollisionTest(temporalObject, bodiesAABBoxHitBody);

            if (!ccdResults.empty()) {
                //determine new body transform to avoid collision
                float timeToFirstHit = (*ccdResults.begin())->getTimeToHit();
                updatedTargetTransform = from.integrate(body.getLinearVelocity(), body.getAngularVelocity(), timeToFirstHit * dt);

                //clamp linear velocity
                float maxLinearVelocityAllowed = body.getCcdMotionThreshold() / dt;
                float maxLinearVelocity = maxLinearVelocityAllowed * MAX_LINEAR_VELOCITY_FACTOR; //avoid to create new CCD contact points in narrow phase
                float currentSpeed = body.getLinearVelocity().length();
                if (currentSpeed > maxLinearVelocity) {
                    body.setVelocity((body.getLinearVelocity() / currentSpeed) * maxLinearVelocity, body.getAngularVelocity());
                }
            }
        }

        body.setTransform(updatedTargetTransform);
    }
}
