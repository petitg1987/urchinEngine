#include <stdexcept>
#include <mutex>
#include <thread>

#include "RigidBody.h"
#include "body/InertiaCalculation.h"

namespace urchin {

    RigidBody::RigidBody(const std::string& id, const PhysicsTransform& transform, const std::shared_ptr<const CollisionShape3D>& shape) :
            AbstractBody(id, transform, shape),
            mass(0.0f),
            invMass(0.0f),
            linearDamping(0.0f),
            angularDamping(0.0f) {
        initializeRigidBody(0.0f, 0.0f, 0.0f,Vector3<float>(1.0f, 1.0f, 1.0f), Vector3<float>(1.0f, 1.0f, 1.0f));
    }

    RigidBody::RigidBody(const RigidBody& rigidBody) :
            AbstractBody(rigidBody),
            mass(0.0f),
            invMass(0.0f),
            linearDamping(0.0f),
            angularDamping(0.0f) {
        initializeRigidBody(rigidBody.getMass(), rigidBody.getLinearDamping(), rigidBody.getAngularDamping(), rigidBody.getLinearFactor(), rigidBody.getAngularFactor());
    }

    RigidBody* RigidBody::upCast(AbstractBody* abstractBody) {
        return dynamic_cast<RigidBody*>(abstractBody);
    }

    const RigidBody* RigidBody::upCast(const AbstractBody* abstractBody) {
        return dynamic_cast<const RigidBody*>(abstractBody);
    }

    void RigidBody::initializeRigidBody(float mass, float linearDamping, float angularDamping,
                                        const Vector3<float>& linearFactor, const Vector3<float>& angularFactor) {
        this->mass = mass;

        this->linearDamping = linearDamping;
        this->angularDamping = angularDamping;

        this->linearFactor = linearFactor;
        this->angularFactor = angularFactor;

        refreshMassProperties();
        refreshInertia();
        refreshWorldInertia();
    }

    void RigidBody::refreshMassProperties() {
        refreshInertia();

        if (MathFunction::isZero(mass)) {
            setIsStatic(true);
            setIsActive(false);
            invMass = 0.0f;
        } else {
            setIsStatic(false);
            setIsActive(true);
            invMass = 1.0f / mass;
        }
    }

    void RigidBody::refreshInertia() {
        this->localInertia = getShape()->computeLocalInertia(mass);
        this->invLocalInertia = Vector3<float>(MathFunction::isZero(localInertia.X) ? 0.0f : 1.0f / localInertia.X,
                                               MathFunction::isZero(localInertia.Y) ? 0.0f : 1.0f / localInertia.Y,
                                               MathFunction::isZero(localInertia.Z) ? 0.0f : 1.0f / localInertia.Z);
        refreshWorldInertia();
    }

    void RigidBody::refreshWorldInertia() {
        invWorldInertia = InertiaCalculation::computeInverseWorldInertia(invLocalInertia, transform);
    }

    void RigidBody::refreshBodyActiveState() {
        if (!isStatic() && !isActive()) {
            setIsActive(bodyMomentum.hasMomentum() || isManuallyMoved);
        }
    }

    void RigidBody::setTransform(const PhysicsTransform& transform) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        if (!(std::this_thread::get_id() == physicsThreadId)) {
            if (AbstractBody::isActive()) {
                throw std::runtime_error("Impossible to update the rigid body position/orientation while physics engine manages it");
            }
            isManuallyMoved = true;
            refreshBodyActiveState();
        }

        this->transform = transform;
        refreshWorldInertia();
    }

    /**
     * Define the body velocity. Undetermined behavior if called outside the physics engine thread.
     */
    void RigidBody::setVelocity(const Vector3<float>& linearVelocity, const Vector3<float>& angularVelocity) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->linearVelocity = linearVelocity;
        this->angularVelocity = angularVelocity;
    }

    Vector3<float> RigidBody::getLinearVelocity() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return linearVelocity;
    }

    Vector3<float> RigidBody::getAngularVelocity() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return angularVelocity;
    }

    void RigidBody::applyCentralMomentum(const Vector3<float>& momentum) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        bodyMomentum.addMomentum(momentum);
        refreshBodyActiveState();
    }

    void RigidBody::applyMomentum(const Vector3<float>& momentum, const Point3<float>& pos) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        bodyMomentum.addMomentum(momentum); //apply central force
        bodyMomentum.addTorqueMomentum(pos.toVector().crossProduct(momentum)); //apply torque
        refreshBodyActiveState();
    }

    void RigidBody::applyTorqueMomentum(const Vector3<float>& torqueMomentum) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        bodyMomentum.addTorqueMomentum(torqueMomentum);
        refreshBodyActiveState();
    }

    /**
     * Return the momentum and reset it. Undetermined behavior if called outside the physics engine thread.
     */
    BodyMomentum RigidBody::getMomentumAndReset() {
        std::lock_guard<std::mutex> lock(bodyMutex);

        BodyMomentum result(bodyMomentum);
        bodyMomentum.reset();
        return result;
    }

    void RigidBody::setMass(float mass) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->mass = mass;
        refreshMassProperties();
    }

    float RigidBody::getMass() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return mass;
    }

    float RigidBody::getInvMass() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return invMass;
    }

    Vector3<float> RigidBody::getLocalInertia() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return localInertia;
    }

    Matrix3<float> RigidBody::getInvWorldInertia() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return invWorldInertia;
    }

    /**
     * Sets damping values. Values of damping must be between 0 and 1.
     * Zero value means no resistance at all and value 1 stop directly the objects.
     * Damping can be used to imitate air resistance.
     */
    void RigidBody::setDamping(float linearDamping, float angularDamping) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        if (linearDamping < 0.0 || linearDamping > 1.0) {
            throw std::domain_error("Wrong linear damping value.");
        }

        if (angularDamping < 0.0 || angularDamping > 1.0) {
            throw std::domain_error("Wrong angular damping value.");
        }

        this->linearDamping = linearDamping;
        this->angularDamping = angularDamping;
    }

    float RigidBody::getLinearDamping() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return linearDamping;
    }

    float RigidBody::getAngularDamping() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return angularDamping;
    }

    /**
     * @param linearFactor Linear factor. Linear factor allows to block movement if axis value is 0.
     */
    void RigidBody::setLinearFactor(const Vector3<float>& linearFactor) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->linearFactor = linearFactor;
    }

    /**
     * @return Linear factor. Linear factor allows to block movement if axis value is 0.
     */
    Vector3<float> RigidBody::getLinearFactor() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return linearFactor;
    }

    /**
     * @param angularFactor Angular factor. Angular factor allows to block rotation movement if axis value is 0.
     */
    void RigidBody::setAngularFactor(const Vector3<float>& angularFactor) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->angularFactor = angularFactor;
    }

    /**
     * @return Angular factor. Angular factor allows to block rotation movement if axis value is 0.
     */
    Vector3<float> RigidBody::getAngularFactor() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return angularFactor;
    }

}
