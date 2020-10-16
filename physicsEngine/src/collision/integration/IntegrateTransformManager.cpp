#include <memory>

#include "collision/integration/IntegrateTransformManager.h"
#include "shape/CollisionSphereShape.h"
#include "object/TemporalObject.h"

#define MAX_LINEAR_VELOCITY_FACTOR 0.95f

namespace urchin {

    IntegrateTransformManager::IntegrateTransformManager(const BodyManager *bodyManager,
            const BroadPhaseManager *broadPhaseManager, const NarrowPhaseManager *narrowPhaseManager) :
            bodyManager(bodyManager),
            broadPhaseManager(broadPhaseManager),
            narrowPhaseManager(narrowPhaseManager) {

    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void IntegrateTransformManager::integrateTransform(float dt) {
        for (auto abstractBody : bodyManager->getWorkBodies()) {
            WorkRigidBody *body = WorkRigidBody::upCast(abstractBody);
            if (body && body->isActive()) {
                const PhysicsTransform &currentTransform = body->getPhysicsTransform();
                PhysicsTransform newTransform = body->getPhysicsTransform().integrate(body->getLinearVelocity(), body->getAngularVelocity(), dt);

                float ccdMotionThreshold = body->getCcdMotionThreshold();
                float motion = currentTransform.getPosition().vector(newTransform.getPosition()).length();

                if (motion > ccdMotionThreshold) {
                    handleContinuousCollision(body, currentTransform, newTransform, dt);
                } else {
                    body->setPosition(newTransform.getPosition());
                    body->setOrientation(newTransform.getOrientation());
                }
            }
        }
    }

    void IntegrateTransformManager::handleContinuousCollision(WorkRigidBody *body, const PhysicsTransform &from, const PhysicsTransform &to, float dt) {
        PhysicsTransform updatedTargetTransform = to;

        std::vector<AbstractWorkBody *> bodiesAABBoxHitBody = broadPhaseManager->bodyTest(body, from, to);
        if (!bodiesAABBoxHitBody.empty()) {
            auto bodyEncompassedSphereShape = std::make_shared<CollisionSphereShape>(body->getShape()->getMinDistanceToCenter());
            TemporalObject temporalObject(bodyEncompassedSphereShape.get(), from, to);
            ccd_set ccdResults = narrowPhaseManager->continuousCollisionTest(temporalObject, bodiesAABBoxHitBody);

            if (!ccdResults.empty()) {
                //determine new body transform to avoid collision
                float timeToFirstHit = (*ccdResults.begin())->getTimeToHit();
                updatedTargetTransform = from.integrate(body->getLinearVelocity(), body->getAngularVelocity(), timeToFirstHit*dt);

                //clamp linear velocity
                float maxLinearVelocityAllowed = body->getCcdMotionThreshold() / dt;
                float maxLinearVelocity = maxLinearVelocityAllowed * MAX_LINEAR_VELOCITY_FACTOR; //avoid to create new CCD contact points in narrow phase
                float currentSpeed = body->getLinearVelocity().length();
                if (currentSpeed > maxLinearVelocity) {
                    body->setLinearVelocity((body->getLinearVelocity() / currentSpeed) * maxLinearVelocity);
                }
            }
        }

        body->setPosition(updatedTargetTransform.getPosition());
        body->setOrientation(updatedTargetTransform.getOrientation());
    }
}
