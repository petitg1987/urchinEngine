#include <utility>

#include "body/work/AbstractWorkBody.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin {

    //static
    uint_fast32_t AbstractWorkBody::nextObjectId = 0;
    bool AbstractWorkBody::bDisableAllBodies = false; //TODO move in AbstractBody ?

    AbstractWorkBody::AbstractWorkBody(std::string id, const PhysicsTransform& physicsTransform, std::shared_ptr<const CollisionShape3D> shape) :
            IslandElement(),
            physicsTransform(physicsTransform),
            shape(std::move(shape)),
            id(std::move(id)),
            restitution(0.0f),
            friction(0.0f),
            rollingFriction(0.0f),
            ccdMotionThreshold(0.0f),
            bIsStatic(true),
            bIsActive(false),
            objectId(nextObjectId++) {

    }

    const PhysicsTransform& AbstractWorkBody::getPhysicsTransform() const {
        return physicsTransform;
    }

    void AbstractWorkBody::setPosition(const Point3<float>& position) {
        physicsTransform.setPosition(position);
    }

    const Point3<float>& AbstractWorkBody::getPosition() const {
        return physicsTransform.getPosition();
    }

    void AbstractWorkBody::setOrientation(const Quaternion<float>& orientation) {
        physicsTransform.setOrientation(orientation);
    }

    const Quaternion<float>& AbstractWorkBody::getOrientation() const {
        return physicsTransform.getOrientation();
    }

    const CollisionShape3D* AbstractWorkBody::getShape() const {
        return shape.get();
    }

    const std::string& AbstractWorkBody::getId() const {
        return id;
    }

    /**
     * @param restitution Restitution (0=stop, 1=100% elastic)
     */
    void AbstractWorkBody::setRestitution(float restitution) {
        this->restitution = restitution;
    }

    /**
     * @return Restitution (0=stop, 1=100% elastic)
     */
    float AbstractWorkBody::getRestitution() const {
        return restitution;
    }

    /**
     * @param friction Friction (0=no friction, 1=total friction)
     */
    void AbstractWorkBody::setFriction(float friction) {
        this->friction = friction;
    }

    /**
     * @return Friction (0=no friction, 1=total friction)
     */
    float AbstractWorkBody::getFriction() const {
        return friction;
    }

    /**
     * @param rollingFriction Rolling friction (0=no friction, 1=total friction)
     */
    void AbstractWorkBody::setRollingFriction(float rollingFriction) {
        this->rollingFriction = rollingFriction;
    }

    /**
     * @return Rolling friction (0=no friction, 1=total friction)
     */
    float AbstractWorkBody::getRollingFriction() const {
        return rollingFriction;
    }

    /**
     * @return Threshold for continuous collision detection in distance unit
     */
    float AbstractWorkBody::getCcdMotionThreshold() const {
        return ccdMotionThreshold;
    }

    /**
     * Process continuous collision detection if the motion in one step is more then threshold.
     * @param ccdMotionThreshold Threshold for continuous collision detection in distance unit.
     */
    void AbstractWorkBody::setCcdMotionThreshold(float ccdMotionThreshold) {
        this->ccdMotionThreshold = ccdMotionThreshold;
    }

    PairContainer* AbstractWorkBody::getPairContainer() const {
        return nullptr;
    }

    void AbstractWorkBody::disableAllBodies(bool value) {
        bDisableAllBodies = value;
    }

    /**
     * @return True when body is static (cannot be affected by physics world)
     */
    bool AbstractWorkBody::isStatic() const {
        return bDisableAllBodies || bIsStatic;
    }

    /**
     * @param bIsStatic Indicate whether body is static (cannot be affected by physics world)
     */
    void AbstractWorkBody::setIsStatic(bool bIsStatic) {
        this->bIsStatic = bIsStatic;
    }

    /**
     * @return True when body is active (body has velocity and/or one of body in same island is active)
     */
    bool AbstractWorkBody::isActive() const {
        return !bDisableAllBodies && bIsActive;
    }

    /**
     * @param bIsActive Indicate whether body is active (body has velocity and/or one of body in same island is active)
     */
    void AbstractWorkBody::setIsActive(bool bIsActive) {
        assert(!(bIsActive && bIsStatic)); //an active body cannot be static

        this->bIsActive = bIsActive;
    }

    uint_fast32_t AbstractWorkBody::getObjectId() const {
        return objectId;
    }

}
