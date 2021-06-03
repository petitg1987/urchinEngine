#include <collision/integration/IntegrateVelocityManager.h>
#include <body/model/RigidBody.h>

namespace urchin {

    IntegrateVelocityManager::IntegrateVelocityManager(const BodyManager* bodyManager) :
        bodyManager(bodyManager) {

    }

    /**
     * @param dt Delta of time (sec.) between two simulation steps
     * @param manifoldResults Constraints to solve
     * @param gravity Gravity expressed in units/s^2
     */
    void IntegrateVelocityManager::integrateVelocity(float dt, const std::vector<OverlappingPair*>& overlappingPairs, const Vector3<float>& gravity) {
        //apply internal forces
        applyGravityForce(gravity, dt);
        applyRollingFrictionResistanceForce(dt, overlappingPairs);

        //integrate velocities and apply damping
        for (auto abstractBody : bodyManager->getBodies()) {
            RigidBody* body = RigidBody::upCast(abstractBody);
            if (body && body->isActive()) {
                float dampingLinearFactor = powf(1.0f - body->getLinearDamping(), dt);
                float dampingAngularFactor = powf(1.0f - body->getAngularDamping(), dt);
                BodyMomentum bodyMomentum = body->getMomentumAndReset();

                Vector3<float> newLinearVelocity = (body->getLinearVelocity() + bodyMomentum.getMomentum() * body->getInvMass()) * dampingLinearFactor;
                Vector3<float> newAngularVelocity = (body->getAngularVelocity() + bodyMomentum.getTorqueMomentum() * body->getInvWorldInertia()) * dampingAngularFactor;
                body->setVelocity(newLinearVelocity, newAngularVelocity);
            }
        }
    }

    /**
     * @param gravity Gravity expressed in units/s^2
     */
    void IntegrateVelocityManager::applyGravityForce(const Vector3<float>& gravity, float dt) {
        for (auto abstractBody : bodyManager->getBodies()) {
            RigidBody* body = RigidBody::upCast(abstractBody);
            if (body && body->isActive()) {
                body->applyCentralMomentum(gravity * body->getMass() * dt);
            }
        }
    }

    void IntegrateVelocityManager::applyRollingFrictionResistanceForce(float dt, const std::vector<OverlappingPair*>& overlappingPairs) {
        for (const auto& overlappingPair : overlappingPairs) {
            float rollingFriction = std::max(overlappingPair->getBody1()->getRollingFriction(), overlappingPair->getBody2()->getRollingFriction());

            for (unsigned int bodyIndex = 0; bodyIndex < 2; ++bodyIndex) {
                RigidBody* body = RigidBody::upCast(overlappingPair->getBody(bodyIndex));
                if (body && body->isActive()) {
                    Matrix3<float> inertia = body->getInvWorldInertia().inverse();
                    Vector3<float> currentTorqueForce = (body->getAngularVelocity() * inertia) / dt;

                    Vector3<float> rollingFrictionForceDirection = -currentTorqueForce.normalize();
                    Vector3<float> rollingFrictionForce = rollingFrictionForceDirection * rollingFriction * body->getMass();

                    for (std::size_t i = 0; i < 3; ++i) {
                        if (fabs(rollingFrictionForce[i]) > fabs(currentTorqueForce[i])) {
                            //If rolling resistance force is higher that torque force: we apply negative torque force in order to stop rolling.
                            //By doing that, we imitate a static friction.
                            rollingFrictionForce[i] = -currentTorqueForce[i];
                        }
                    }

                    body->applyTorqueMomentum(rollingFrictionForce * dt);
                }
            }
        }
    }

}
