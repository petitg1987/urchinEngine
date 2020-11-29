#include <stdexcept>
#include <limits>
#include <mutex>

#include "RigidBody.h"

namespace urchin {

    RigidBody::RigidBody(const std::string& id, const Transform<float>& transform, const std::shared_ptr<const CollisionShape3D>& shape) :
            AbstractBody(id, transform, shape),
            mass(0.0f),
            linearDamping(0.0f),
            angularDamping(0.0f) {
        initializeRigidBody(0.0f, 0.0f, 0.0f,
                Vector3<float>(1.0f, 1.0f, 1.0f), Vector3<float>(1.0f, 1.0f, 1.0f));
    }

    RigidBody::RigidBody(const RigidBody& rigidBody) :
        AbstractBody(rigidBody),
        mass(0.0f),
        linearDamping(0.0f),
        angularDamping(0.0f) {
        initializeRigidBody(rigidBody.getMass(), rigidBody.getLinearDamping(), rigidBody.getAngularDamping(),
                rigidBody.getLinearFactor(), rigidBody.getAngularFactor());
    }

    void RigidBody::initializeRigidBody(float mass, float linearDamping, float angularDamping,
            const Vector3<float>& linearFactor, const Vector3<float>& angularFactor) {
        this->mass = mass;
        refreshMassProperties();

        this->linearDamping = linearDamping;
        this->angularDamping = angularDamping;

        this->linearFactor = linearFactor;
        this->angularFactor = angularFactor;
    }

    void RigidBody::refreshScaledShape() {
        AbstractBody::refreshScaledShape();

        refreshLocalInertia();
    }

    void RigidBody::refreshMassProperties() {
        refreshLocalInertia();
        setIsStatic(mass > -std::numeric_limits<float>::epsilon() && mass < std::numeric_limits<float>::epsilon());
    }

    void RigidBody::refreshLocalInertia() {
        this->localInertia = computeScaledShapeLocalInertia(mass);
    }

    AbstractWorkBody *RigidBody::createWorkBody() const {
        const Transform<float>& transform = getTransform();
        PhysicsTransform physicsTransform(transform.getPosition(), transform.getOrientation());

        auto* workRigidBody = new WorkRigidBody(getId(), physicsTransform, getScaledShape());
        workRigidBody->setMassProperties(getMass(), getLocalInertia());
        workRigidBody->setLinearVelocity(getLinearVelocity());
        workRigidBody->setAngularVelocity(getAngularVelocity());
        return workRigidBody;
    }

    void RigidBody::updateTo(AbstractWorkBody* workBody) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        AbstractBody::updateTo(workBody);

        WorkRigidBody* workRigidBody = WorkRigidBody::upCast(workBody);
        if (workRigidBody) {
            workRigidBody->setTotalMomentum(totalMomentum);
            workRigidBody->setTotalTorqueMomentum(totalTorqueMomentum);
            workRigidBody->setDamping(linearDamping, angularDamping);
            workRigidBody->setLinearFactor(linearFactor);
            workRigidBody->setAngularFactor(angularFactor);

            workRigidBody->refreshInvWorldInertia();

            totalMomentum.setNull();
            totalTorqueMomentum.setNull();
        }
    }

    bool RigidBody::applyFrom(const AbstractWorkBody* workBody) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        bool fullRefreshRequested = AbstractBody::applyFrom(workBody);
        const WorkRigidBody* workRigidBody = WorkRigidBody::upCast(workBody);

        if (workRigidBody && !fullRefreshRequested) {
            mass = workRigidBody->getMass();
            refreshMassProperties();

            linearVelocity = workRigidBody->getLinearVelocity();
            angularVelocity = workRigidBody->getAngularVelocity();
        }

        return fullRefreshRequested;
    }

    Vector3<float> RigidBody::getLinearVelocity() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return linearVelocity;
    }

    Vector3<float> RigidBody::getAngularVelocity() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return angularVelocity;
    }

    Vector3<float> RigidBody::getTotalMomentum() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return totalMomentum;
    }

    void RigidBody::applyCentralMomentum(const Vector3<float>& momentum) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        totalMomentum += momentum;
    }

    void RigidBody::applyMomentum(const Vector3<float>& momentum, const Point3<float>& pos) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        //apply central force
        totalMomentum += momentum;

        //apply torque
        totalTorqueMomentum += pos.toVector().crossProduct(momentum);
    }

    Vector3<float> RigidBody::getTotalTorqueMomentum() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return totalTorqueMomentum;
    }

    void RigidBody::applyTorqueMomentum(const Vector3<float>& torqueMomentum) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        totalTorqueMomentum += torqueMomentum;
    }

    void RigidBody::setMass(float mass) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->mass = mass;
        refreshMassProperties();
        this->setNeedFullRefresh(true);
    }

    float RigidBody::getMass() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return mass;
    }

    Vector3<float> RigidBody::getLocalInertia() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return localInertia;
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
