#include "collision/integration/IntegrateTransform.h"
#include "shape/CollisionSphereShape.h"
#include "object/TemporalObject.h"

namespace urchin {

    IntegrateTransform::IntegrateTransform(const BodyContainer& bodyContainer, const BroadPhase& broadPhase, const NarrowPhase& narrowPhase) :
            bodyContainer(bodyContainer),
            broadPhase(broadPhase),
            narrowPhase(narrowPhase) {

    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void IntegrateTransform::process(float dt) const {
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

    void IntegrateTransform::handleContinuousCollision(RigidBody& body, const PhysicsTransform& from, const PhysicsTransform& to, float dt) const {
        PhysicsTransform updatedTargetTransform = to;

        std::vector<std::shared_ptr<AbstractBody>> bodiesAABBoxHitBody;
        bodiesAABBoxHitBody.reserve(10);
        broadPhase.bodyTest(body, from, to, bodiesAABBoxHitBody);

        if (!bodiesAABBoxHitBody.empty()) {
            CollisionSphereShape bodyEncompassedSphereShape(body.getShape().getMinDistanceToCenter());
            TemporalObject temporalObject(bodyEncompassedSphereShape, 0, from, to);
            std::vector<ContinuousCollisionResult<float>> ccdResults;
            narrowPhase.continuousCollisionTest(temporalObject, bodiesAABBoxHitBody, ccdResults);

            if (!ccdResults.empty()) {
                //determine new body transform to avoid collision
                float timeToFirstHit = std::ranges::min_element(ccdResults, ContinuousCollisionResultComparator<float>())->getTimeToHit();
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
