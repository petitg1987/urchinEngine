#include <limits>

#include "body/work/WorkRigidBody.h"
#include "body/InertiaCalculation.h"

namespace urchin {

    WorkRigidBody::WorkRigidBody(const std::string& id, const PhysicsTransform& physicsTransform, const std::shared_ptr<const CollisionShape3D>& shape) :
            AbstractWorkBody(id, physicsTransform, shape),
            mass(0.0f),
            invMass(0.0f),
            invLocalInertia(Vector3<float>(0.0, 0.0, 0.0)),
            invWorldInertia(Matrix3<float>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)),
            linearDamping(0.0),
            angularDamping(0.0) {

    }

    WorkRigidBody* WorkRigidBody::upCast(AbstractWorkBody* workBody) {
        return dynamic_cast<WorkRigidBody*>(workBody);
    }

    const WorkRigidBody* WorkRigidBody::upCast(const AbstractWorkBody* workBody) {
        return dynamic_cast<const WorkRigidBody*>(workBody);
    }

    /**
     * Refresh body active state. If forces are apply on body: active body
     */
    void WorkRigidBody::refreshBodyActiveState() {
        if (!isStatic() && !isActive()) {
            if (totalMomentum.squareLength() > std::numeric_limits<float>::epsilon()
                    || totalMomentum.squareLength() > std::numeric_limits<float>::epsilon()) {
                setIsActive(true);
            }
        }
    }

    void WorkRigidBody::setLinearVelocity(const Vector3<float>& linearVelocity) {
        this->linearVelocity = linearVelocity;
    }

    const Vector3<float>& WorkRigidBody::getLinearVelocity() const {
        return linearVelocity;
    }

    void WorkRigidBody::setAngularVelocity(const Vector3<float>& angularVelocity) {
        this->angularVelocity = angularVelocity;
    }

    const Vector3<float>& WorkRigidBody::getAngularVelocity() const {
        return angularVelocity;
    }

    const Vector3<float>& WorkRigidBody::getTotalMomentum() const {
        return totalMomentum;
    }

    void WorkRigidBody::setTotalMomentum(const Vector3<float>& totalMomentum) {
        this->totalMomentum = totalMomentum;

        refreshBodyActiveState();
    }

    void WorkRigidBody::applyCentralMomentum(const Vector3<float>& momentum) {
        totalMomentum += momentum * linearFactor;
    }

    void WorkRigidBody::applyMomentum(const Vector3<float>& momentum, const Point3<float>& momentumPoint) {
        //apply central force
        totalMomentum += momentum * linearFactor;

        //apply torque
        totalTorqueMomentum += momentumPoint.toVector().crossProduct(momentum * linearFactor);
    }

    void WorkRigidBody::resetMomentum() {
        totalMomentum.setValues(0.0, 0.0, 0.0);
    }

    const Vector3<float>& WorkRigidBody::getTotalTorqueMomentum() const {
        return totalTorqueMomentum;
    }

    void WorkRigidBody::setTotalTorqueMomentum(const Vector3<float>& totalTorqueMomentum) {
        this->totalTorqueMomentum = totalTorqueMomentum;

        refreshBodyActiveState();
    }

    void WorkRigidBody::applyTorqueMomentum(const Vector3<float>& torqueMomentum) {
        totalTorqueMomentum += torqueMomentum * angularFactor;
    }

    void WorkRigidBody::resetTorqueMomentum() {
        totalTorqueMomentum.setValues(0.0, 0.0, 0.0);
    }

    void WorkRigidBody::setMassProperties(float mass, const Vector3<float>& localInertia) {
        if (MathFunction::isZero(mass)) {
            setIsStatic(true);
            setIsActive(false);
            this->invMass = 0.0f;
        } else {
            if (isStatic()) //avoid wake up of body (isActive flag) when static flag is already correct
            {
                setIsStatic(false);
                setIsActive(true);
            }
            this->invMass = 1.0f / mass;
        }
        this->mass = mass;

        this->invLocalInertia.X = MathFunction::isZero(localInertia.X) ? 0.0f : 1.0f / localInertia.X;
        this->invLocalInertia.Y = MathFunction::isZero(localInertia.Y) ? 0.0f : 1.0f / localInertia.Y;
        this->invLocalInertia.Z = MathFunction::isZero(localInertia.Z) ? 0.0f : 1.0f / localInertia.Z;
    }

    float WorkRigidBody::getMass() const {
        return mass;
    }

    float WorkRigidBody::getInvMass() const {
        return invMass;
    }

    /**
     * Refresh inverse world inertia based on body transformation
     */
    void WorkRigidBody::refreshInvWorldInertia() {
        if (!isStatic()) {
            invWorldInertia = InertiaCalculation::computeInverseWorldInertia(invLocalInertia, getPhysicsTransform());
        } else {
            invWorldInertia = Matrix3<float>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        }
    }

    const Vector3<float>& WorkRigidBody::getInvLocalInertia() const {
        return invLocalInertia;
    }

    const Matrix3<float>& WorkRigidBody::getInvWorldInertia() const {
        return invWorldInertia;
    }

    /**
     * Sets damping values. Values of damping must be between 0 and 1.
     * Zero value means no resistance at all and value 1 stop directly the objects.
     */
    void WorkRigidBody::setDamping(float linearDamping, float angularDamping) {
        this->linearDamping = linearDamping;
        this->angularDamping = angularDamping;
    }

    float WorkRigidBody::getLinearDamping() const {
        return linearDamping;
    }

    float WorkRigidBody::getAngularDamping() const {
        return angularDamping;
    }

    /**
     * @param linearFactor Linear factor. Linear factor allows to block movement if axis value is 0.
     */
    void WorkRigidBody::setLinearFactor(const Vector3<float>& linearFactor) {
        this->linearFactor = linearFactor;
    }

    /**
     * @return Linear factor. Linear factor allows to block movement if axis value is 0.
     */
    const Vector3<float>& WorkRigidBody::getLinearFactor() const {
        return linearFactor;
    }

    /**
     * @param angularFactor Angular factor. Angular factor allows to block rotation movement if axis value is 0.
     */
    void WorkRigidBody::setAngularFactor(const Vector3<float>& angularFactor) {
        this->angularFactor = angularFactor;
    }

    /**
     * @return Angular factor. Angular factor allows to block rotation movement if axis value is 0.
     */
    const Vector3<float>& WorkRigidBody::getAngularFactor() const {
        return angularFactor;
    }

    void WorkRigidBody::setIsStatic(bool bIsStatic) {
        if (bIsStatic) {
            makeBodyStatic();
        } else {
            AbstractWorkBody::setIsStatic(false);
        }
    }

    bool WorkRigidBody::isGhostBody() const {
        return false;
    }

    void WorkRigidBody::makeBodyStatic() {
        AbstractWorkBody::setIsStatic(true);
        AbstractWorkBody::setIsActive(false);
        setLinearVelocity(Vector3<float>(0.0f, 0.0f, 0.0f));
        setAngularVelocity(Vector3<float>(0.0f, 0.0f, 0.0f));
    }

}
