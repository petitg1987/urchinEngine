#include <collision/integration/IntegrateTransform.h>
#include <shape/CollisionSphereShape.h>
#include <object/TemporalObject.h>

namespace urchin {

    IntegrateTransform::IntegrateTransform(const BodyContainer& bodyContainer, const BroadPhase& broadPhase, const NarrowPhase& narrowPhase) :
            bodyContainer(bodyContainer),
            broadPhase(broadPhase),
            narrowPhase(narrowPhase) {

    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void IntegrateTransform::process(float dt) {
        for (const auto& abstractBody : bodyContainer.getBodies()) {
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

    void IntegrateTransform::handleContinuousCollision(RigidBody& body, const PhysicsTransform& from, const PhysicsTransform& to, float dt) {
        PhysicsTransform updatedTargetTransform = to;

        std::vector<std::shared_ptr<AbstractBody>> bodiesAABBoxHitBody = broadPhase.bodyTest(body, from, to);
        if (!bodiesAABBoxHitBody.empty()) {
            CollisionSphereShape bodyEncompassedSphereShape(body.getShape().getMinDistanceToCenter());
            TemporalObject temporalObject(bodyEncompassedSphereShape, from, to);
            ccd_set ccdResults = narrowPhase.continuousCollisionTest(temporalObject, bodiesAABBoxHitBody);

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
