#include <utility>

#include <body/model/AbstractBody.h>
#include <collision/broadphase/PairContainer.h>

namespace urchin {

    //static
    uint_fast32_t AbstractBody::nextObjectId = 0;
    bool AbstractBody::bDisableAllBodies = false;

    AbstractBody::AbstractBody(BodyType bodyType, std::string id, const PhysicsTransform& transform, std::unique_ptr<const CollisionShape3D> shape) :
            transform(transform),
            isManuallyMoved(false),
            bodyType(bodyType),
            id(std::move(id)),
            shape(std::move(shape)),
            restitution(0.0f),
            friction(0.0f),
            rollingFriction(0.0f),
            ccdMotionThreshold(0.0f),
            bIsStatic(true),
            bIsActive(false),
            objectId(nextObjectId++) {
        initialize(0.2f, 0.5f, 0.0f);
    }

    AbstractBody::AbstractBody(const AbstractBody& abstractBody) :
            IslandElement(abstractBody),
            transform(abstractBody.getTransform()),
            isManuallyMoved(false),
            bodyType(abstractBody.bodyType),
            id(abstractBody.getId()),
            shape(abstractBody.getShape().clone()),
            restitution(0.0f),
            friction(0.0f),
            rollingFriction(0.0f),
            ccdMotionThreshold(0.0f),
            bIsStatic(true),
            bIsActive(false),
            objectId(nextObjectId++) {
        initialize(abstractBody.getRestitution(), abstractBody.getFriction(), abstractBody.getRollingFriction());
        setCcdMotionThreshold(abstractBody.getCcdMotionThreshold()); //override default value
    }

    void AbstractBody::setPhysicsThreadId(std::thread::id physicsThreadId) {
        this->physicsThreadId = physicsThreadId;
    }

    void AbstractBody::initialize(float restitution, float friction, float rollingFriction) {
        //technical data
        bIsStatic.store(true, std::memory_order_release);
        bIsActive.store(false, std::memory_order_release);

        //shape check and data
        shape->checkInnerMarginQuality(id);
        ccdMotionThreshold = shape->getMinDistanceToCenter() * CCD_MOTION_THRESHOLD_FACTOR;

        //body description data
        this->restitution = restitution;
        this->friction = friction;
        this->rollingFriction = rollingFriction;
    }

    void AbstractBody::setTransform(const PhysicsTransform& transform) {
        std::scoped_lock lock(bodyMutex);
        this->transform = transform;
    }

    PhysicsTransform AbstractBody::getTransform() const {
        std::scoped_lock lock(bodyMutex);
        return transform;
    }

    bool AbstractBody::getManuallyMovedAndReset() {
        bool expected = true;
        isManuallyMoved.compare_exchange_strong(expected, false);
        return expected;
    }

    const CollisionShape3D& AbstractBody::getShape() const {
        return *shape;
    }

    BodyType AbstractBody::getBodyType() const {
        return bodyType;
    }

    void AbstractBody::setId(std::string id) {
        this->id = std::move(id);
    }

    const std::string& AbstractBody::getId() const {
        return id;
    }

    void AbstractBody::setRestitution(float restitution) {
        std::scoped_lock<std::mutex> lock(bodyMutex);
        this->restitution = restitution;
    }

    /**
     * @return Restitution (0=stop, 1=100% elastic)
     */
    float AbstractBody::getRestitution() const {
        std::scoped_lock lock(bodyMutex);
        return restitution;
    }

    void AbstractBody::setFriction(float friction) {
        std::scoped_lock lock(bodyMutex);
        this->friction = friction;
    }

    /**
     * @return Friction (0=no friction, 1=total friction)
     */
    float AbstractBody::getFriction() const {
        std::scoped_lock lock(bodyMutex);
        return friction;
    }

    void AbstractBody::setRollingFriction(float rollingFriction) {
        std::scoped_lock lock(bodyMutex);
        this->rollingFriction = rollingFriction;
    }

    /**
     * @return Rolling friction (0=no friction, 1=total friction)
     */
    float AbstractBody::getRollingFriction() const {
        std::scoped_lock lock(bodyMutex);
        return rollingFriction;
    }

    void AbstractBody::setCcdMotionThreshold(float ccdMotionThreshold) {
        std::scoped_lock lock(bodyMutex);
        this->ccdMotionThreshold = ccdMotionThreshold;
    }

    /**
     * @return Threshold for continuous collision detection in distance unit (process continuous collision detection if the motion in one
     * step is more than threshold). A default value is determinate automatically for each body thanks to constant 'CCD_MOTION_THRESHOLD_FACTOR'.
     */
    float AbstractBody::getCcdMotionThreshold() const {
        std::scoped_lock lock(bodyMutex);
        return ccdMotionThreshold;
    }

    PairContainer* AbstractBody::getPairContainer() const {
        return nullptr;
    }

    void AbstractBody::disableAllBodies(bool value) {
        bDisableAllBodies = value;
    }

    void AbstractBody::setIsStatic(bool bIsStatic) {
        this->bIsStatic.store(bIsStatic, std::memory_order_release);
    }

    /**
     * @return Body static state (static body cannot be affected by the physics world)
     */
    bool AbstractBody::isStatic() const {
        return bDisableAllBodies || bIsStatic.load(std::memory_order_acquire);
    }

    /**
     * Define body active state (active body has velocity and/or one of body in the same island is active).
     * Undetermined behavior if called outside the physics engine thread.
     */
    void AbstractBody::setIsActive(bool bIsActive) {
        assert(!(bIsActive && bIsStatic)); //an active body cannot be static
        this->bIsActive.store(bIsActive, std::memory_order_release);
    }

    /**
     * @return Body active state (active body has velocity and/or one of body in the same island is active).
     * Note that if body transform is manually updated, the body is not considered as active.
     */
    bool AbstractBody::isActive() const {
        return !bDisableAllBodies && bIsActive.load(std::memory_order_acquire);
    }

    uint_fast32_t AbstractBody::getObjectId() const {
        return objectId;
    }

}
