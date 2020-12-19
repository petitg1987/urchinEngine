#include <stdexcept>
#include <utility>

#include "AbstractBody.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin {

    //static
    uint_fast32_t AbstractBody::nextObjectId = 0;
    bool AbstractBody::bDisableAllBodies = false;

    AbstractBody::AbstractBody(std::string id, const PhysicsTransform& transform, std::shared_ptr<const CollisionShape3D> shape) :
            ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")),
            bNeedFullRefresh(false),
            transform(transform),
            isManuallyMoved(false),
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
            ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")),
            bNeedFullRefresh(false),
            transform(abstractBody.getTransform()),
            isManuallyMoved(false),
            id(abstractBody.getId()),
            shape(std::shared_ptr<const CollisionShape3D>(abstractBody.getShape()->clone())),
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

    void AbstractBody::setNeedFullRefresh(bool needFullRefresh) {
        this->bNeedFullRefresh.store(needFullRefresh, std::memory_order_relaxed);
    }

    bool AbstractBody::needFullRefresh() const {
        return bNeedFullRefresh.load(std::memory_order_relaxed);
    }

    void AbstractBody::setPhysicsThreadId(std::thread::id physicsThreadId) {
        this->physicsThreadId = physicsThreadId;
    }

    void AbstractBody::initialize(float restitution, float friction, float rollingFriction) {
        //technical data
        bIsStatic.store(true, std::memory_order_relaxed);
        bIsActive.store(false, std::memory_order_relaxed);

        //shape check and data
        shape->checkInnerMarginQuality(id);
        ccdMotionThreshold = (shape->getMinDistanceToCenter() * 2.0f) * ccdMotionThresholdFactor;

        //body description data
        this->restitution = restitution;
        this->friction = friction;
        this->rollingFriction = rollingFriction;
    }

    void AbstractBody::setTransform(const PhysicsTransform& transform) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->transform = transform;
    }

    PhysicsTransform AbstractBody::getTransform() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return transform;
    }

    bool AbstractBody::getManuallyMovedAndReset() {
        if (isManuallyMoved) {
            isManuallyMoved = false;
            return true;
        }
        return false;
    }

    const std::shared_ptr<const CollisionShape3D>& AbstractBody::getShape() const {
        return shape;
    }

    void AbstractBody::setId(const std::string& id) {
        this->id = id;
    }

    const std::string& AbstractBody::getId() const {
        return id;
    }

    void AbstractBody::setRestitution(float restitution) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->restitution = restitution;
    }

    /**
     * @return Restitution (0=stop, 1=100% elastic)
     */
    float AbstractBody::getRestitution() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return restitution;
    }

    void AbstractBody::setFriction(float friction) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->friction = friction;
    }

    /**
     * @return Friction (0=no friction, 1=total friction)
     */
    float AbstractBody::getFriction() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return friction;
    }

    void AbstractBody::setRollingFriction(float rollingFriction) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->rollingFriction = rollingFriction;
    }

    /**
     * @return Rolling friction (0=no friction, 1=total friction)
     */
    float AbstractBody::getRollingFriction() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return rollingFriction;
    }

    void AbstractBody::setCcdMotionThreshold(float ccdMotionThreshold) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->ccdMotionThreshold = ccdMotionThreshold;
    }

    /**
     * @return Threshold for continuous collision detection in distance unit (process continuous collision detection if the motion in one
     * step is more then threshold). A default value is determinate automatically for each body thanks to properties 'collisionShape.ccdMotionThresholdFactor'.
     */
    float AbstractBody::getCcdMotionThreshold() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return ccdMotionThreshold;
    }

    PairContainer* AbstractBody::getPairContainer() const {
        return nullptr;
    }

    void AbstractBody::disableAllBodies(bool value) {
        bDisableAllBodies = value;
    }

    void AbstractBody::setIsStatic(bool bIsStatic) {
        this->bIsStatic.store(bIsStatic, std::memory_order_relaxed);
    }

    /**
     * @return Body static state (static body cannot be affected by the physics world)
     */
    bool AbstractBody::isStatic() const {
        return bDisableAllBodies || bIsStatic.load(std::memory_order_relaxed);
    }

    /**
     * Define body active state (active body has velocity and/or one of body in the same island is active).
     * Undetermined behavior if called outside the physics engine thread.
     */
    void AbstractBody::setIsActive(bool bIsActive) {
        assert(!(bIsActive && bIsStatic)); //an active body cannot be static
        this->bIsActive.store(bIsActive, std::memory_order_relaxed);
    }

    /**
     * @return Body active state (active body has velocity and/or one of body in the same island is active)
     */
    bool AbstractBody::isActive() const {
        return !bDisableAllBodies && bIsActive.load(std::memory_order_relaxed);
    }

    bool AbstractBody::isGhostBody() const {
        return false;
    }

    uint_fast32_t AbstractBody::getObjectId() const {
        return objectId;
    }

}
