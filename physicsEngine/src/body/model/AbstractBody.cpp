#include <stdexcept>
#include <limits>
#include <utility>

#include "AbstractBody.h"

namespace urchin {

    //static
    uint_fast32_t AbstractBody::nextObjectId = 0;
    bool AbstractBody::bDisableAllBodies = false;

    AbstractBody::AbstractBody(std::string id, Transform<float> transform, std::shared_ptr<const CollisionShape3D> shape) :
            ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")),
            transform(std::move(transform)),
            isManuallyMoved(false),
            id(std::move(id)),
            originalShape(std::move(shape)),
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
            transform(abstractBody.getTransform()),
            isManuallyMoved(false),
            id(abstractBody.getId()),
            originalShape(std::shared_ptr<const CollisionShape3D>(abstractBody.getOriginalShape()->clone())),
            restitution(0.0f),
            friction(0.0f),
            rollingFriction(0.0f),
            ccdMotionThreshold(0.0f),
            bIsStatic(true),
            bIsActive(false),
            objectId(nextObjectId++) {
        initialize(abstractBody.getRestitution(), abstractBody.getFriction(), abstractBody.getRollingFriction());
        setCcdMotionThreshold(abstractBody.getCcdMotionThreshold());
    }

    void AbstractBody::initialize(float restitution, float friction, float rollingFriction) {
        //technical data
        bIsStatic.store(true, std::memory_order_relaxed);
        bIsActive.store(false, std::memory_order_relaxed);

        //body description data
        refreshScaledShape();
        this->restitution = restitution;
        this->friction = friction;
        this->rollingFriction = rollingFriction;
    }

    void AbstractBody::refreshScaledShape() {
        scaledShape = originalShape->scale(transform.getScale());
        scaledShape->checkInnerMarginQuality(id);

        ccdMotionThreshold = (scaledShape->getMinDistanceToCenter() * 2.0f) * ccdMotionThresholdFactor;
    }

//    bool AbstractBody::applyFrom(const AbstractWorkBody* workBody) { //TODO remove
//        #ifndef NDEBUG
//            assert(!bodyMutex.try_lock()); //body mutex should be locked before call this method
//        #endif
//
//        bool fullRefreshRequested = bNeedFullRefresh.load(std::memory_order_relaxed);
//        if (!fullRefreshRequested) {
//            bIsActive.store(workBody->isActive(), std::memory_order_relaxed);
//
//            transform.setPosition(workBody->getPosition());
//            transform.setOrientation(workBody->getOrientation());
//        }
//
//        return fullRefreshRequested;
//    }

    void AbstractBody::setTransform(const Transform<float>& transform) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        if (std::abs(transform.getScale() - this->transform.getScale()) > std::numeric_limits<float>::epsilon()) {
            this->transform = transform;
            refreshScaledShape();
        } else {
            this->transform = transform;
        }

        //this->setNeedFullRefresh(true); //TODO send ADD_BODY & REMOVE_BODY event ?
        this->isManuallyMoved = true;
    }

    Transform<float> AbstractBody::getTransform() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return transform;
    }

    PhysicsTransform AbstractBody::getPhysicsTransform() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return PhysicsTransform(transform.getPosition(), transform.getOrientation());
    }

    void AbstractBody::setPosition(const Point3<float>& position) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        transform.setPosition(position);
    }

    Point3<float> AbstractBody::getPosition() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return transform.getPosition();
    }

    void AbstractBody::setOrientation(const Quaternion<float>& orientation) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        transform.setOrientation(orientation); //TODO transform.setOrientation has bad performance compare to physicsTrasnform.setOrientation !
    }

    Quaternion<float> AbstractBody::getOrientation() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

        return transform.getOrientation();
    }

    bool AbstractBody::isManuallyMovedAndResetFlag() {
        if (isManuallyMoved) {
            isManuallyMoved = false;
            return true;
        }
        return false;
    }

    void AbstractBody::setShape(const std::shared_ptr<const CollisionShape3D>& shape) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->originalShape = shape;
        refreshScaledShape();
        //this->setNeedFullRefresh(true); //TODO send ADD_BODY & REMOVE_BODY event ?
    }

    const std::shared_ptr<const CollisionShape3D>& AbstractBody::getOriginalShape() const {
        std::lock_guard<std::mutex> lock(bodyMutex); //TODO remove if cannot update when physics thread is running

        return originalShape;
    }

    const std::shared_ptr<const CollisionShape3D>& AbstractBody::getScaledShape() const {
        std::lock_guard<std::mutex> lock(bodyMutex); //TODO remove if cannot update when physics thread is running

        return scaledShape;
    }

    /**
     * @return Scaled shape representing the form of the body. The shape is
     * scaled according to scale define in 'transform' attribute.
     */
    const CollisionShape3D* AbstractBody::getShape() const {
        std::lock_guard<std::mutex> lock(bodyMutex); //TODO remove if cannot update when physics thread is running

        return scaledShape.get(); //TODO better to return shared_ptr ?
    }

    Vector3<float> AbstractBody::computeScaledShapeLocalInertia(float mass) const {
        return scaledShape->computeLocalInertia(mass);
    }

    void AbstractBody::setId(const std::string& id) {
        std::lock_guard<std::mutex> lock(bodyMutex);

        this->id = id;
    }

    const std::string& AbstractBody::getId() const {
        std::lock_guard<std::mutex> lock(bodyMutex);

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
        setIsActive(false);
    }

    /**
     * @return True when body is static (cannot be affected by physics world)
     */
    bool AbstractBody::isStatic() const {
        return bDisableAllBodies || bIsStatic.load(std::memory_order_relaxed);
    }

    void AbstractBody::setIsActive(bool bIsActive) {
        assert(!(bIsActive && bIsStatic)); //an active body cannot be static

        this->bIsActive = bIsActive;
    }

    /**
     * @return True when body is active (body has velocity and/or one of body in same island is active)
     */
    bool AbstractBody::isActive() const {
        return !bDisableAllBodies && bIsActive.load(std::memory_order_relaxed);
    }

    uint_fast32_t AbstractBody::getObjectId() const {
        return objectId;
    }

}
