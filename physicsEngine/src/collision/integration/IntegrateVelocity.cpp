#include <collision/integration/IntegrateVelocity.h>
#include <body/model/RigidBody.h>

namespace urchin {

    IntegrateVelocity::IntegrateVelocity(const BodyContainer& bodyContainer) :
            bodyContainer(bodyContainer) {

    }

    /**
     * @param dt Delta of time (sec.) between two simulation steps
     * @param gravity Gravity expressed in units/s^2
     */
    void IntegrateVelocity::process(float dt, const std::vector<std::unique_ptr<OverlappingPair>>& overlappingPairs, const Vector3<float>& gravity) const {
        //apply internal forces
        applyGravityForce(gravity, dt);
        applyRollingFrictionResistanceForce(dt, overlappingPairs);

        //integrate velocities and apply damping
        for (const auto& abstractBody : bodyContainer.getBodies()) {
            RigidBody* body = RigidBody::upCast(abstractBody.get());
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
    void IntegrateVelocity::applyGravityForce(const Vector3<float>& gravity, float dt) const {
        for (const auto& abstractBody : bodyContainer.getBodies()) {
            RigidBody* body = RigidBody::upCast(abstractBody.get());
            if (body && body->isActive()) {
                body->applyCentralMomentum(gravity * body->getMass() * dt);
            }
        }
    }

    void IntegrateVelocity::applyRollingFrictionResistanceForce(float dt, const std::vector<std::unique_ptr<OverlappingPair>>& overlappingPairs) const {
        for (const auto& overlappingPair : overlappingPairs) {
            float rollingFriction = std::max(overlappingPair->getBody1().getRollingFriction(), overlappingPair->getBody2().getRollingFriction());

            for (unsigned int bodyIndex = 0; bodyIndex < 2; ++bodyIndex) {
                RigidBody* body = RigidBody::upCast(&overlappingPair->getBody(bodyIndex));
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
