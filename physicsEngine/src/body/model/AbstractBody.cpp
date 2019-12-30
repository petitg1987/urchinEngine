#include <stdexcept>
#include <limits>
#include <utility>

#include "AbstractBody.h"

namespace urchin
{

	AbstractBody::AbstractBody(std::string id, Transform<float> transform, std::shared_ptr<const CollisionShape3D> shape) :
			ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")),
            bIsNew(false),
            bIsDeleted(false),
            bNeedFullRefresh(false),
			workBody(nullptr),
			transform(std::move(transform)),
			isManuallyMoved(false),
			id(std::move(id)),
            originalShape(std::move(shape)),
            restitution(0.0f),
            friction(0.0f),
            rollingFriction(0.0f),
            ccdMotionThreshold(0.0f),
            bIsStatic(true),
            bIsActive(false)
	{
		initialize(0.2f, 0.5f, 0.0f);
	}

	AbstractBody::AbstractBody(const AbstractBody &abstractBody) :
			ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")),
            bIsNew(false),
            bIsDeleted(false),
            bNeedFullRefresh(false),
			workBody(nullptr),
			transform(abstractBody.getTransform()),
			isManuallyMoved(false),
			id(abstractBody.getId()),
			originalShape(std::shared_ptr<const CollisionShape3D>(abstractBody.getOriginalShape()->clone())),
            restitution(0.0f),
            friction(0.0f),
            rollingFriction(0.0f),
            ccdMotionThreshold(0.0f),
            bIsStatic(true),
            bIsActive(false)
	{
		initialize(abstractBody.getRestitution(), abstractBody.getFriction(), abstractBody.getRollingFriction());
		setCcdMotionThreshold(abstractBody.getCcdMotionThreshold());
	}

	void AbstractBody::initialize(float restitution, float friction, float rollingFriction)
	{
		//technical data
		bIsNew.store(false, std::memory_order_relaxed);
		bIsDeleted.store(false, std::memory_order_relaxed);
		bNeedFullRefresh.store(false, std::memory_order_relaxed);
		bIsStatic.store(true, std::memory_order_relaxed);
		bIsActive.store(false, std::memory_order_relaxed);

		//body description data
		refreshScaledShape();
		this->restitution = restitution;
		this->friction = friction;
		this->rollingFriction = rollingFriction;
	}

	void AbstractBody::refreshScaledShape()
	{
		scaledShape = originalShape->scale(transform.getScale());
		scaledShape->checkInnerMarginQuality(id);

		ccdMotionThreshold = (scaledShape->getMinDistanceToCenter() * 2.0f) * ccdMotionThresholdFactor;
	}

	void AbstractBody::setIsNew(bool isNew)
	{
		this->bIsNew.store(isNew, std::memory_order_relaxed);
	}

	bool AbstractBody::isNew() const
	{
		return bIsNew.load(std::memory_order_relaxed);
	}

	void AbstractBody::markAsDeleted()
	{
		this->bIsDeleted.store(true, std::memory_order_relaxed);
	}

	bool AbstractBody::isDeleted() const
	{
		return bIsDeleted.load(std::memory_order_relaxed);
	}

	void AbstractBody::setNeedFullRefresh(bool needFullRefresh)
	{
		this->bNeedFullRefresh.store(needFullRefresh, std::memory_order_relaxed);
	}

	bool AbstractBody::needFullRefresh() const
	{
		return bNeedFullRefresh.load(std::memory_order_relaxed);
	}

	void AbstractBody::setWorkBody(AbstractWorkBody *workBody)
	{
		this->workBody = workBody;
	}

	AbstractWorkBody *AbstractBody::getWorkBody() const
	{
		return workBody;
	}

	void AbstractBody::updateTo(AbstractWorkBody *workBody)
	{
		#ifndef NDEBUG
	        assert(!bodyMutex.try_lock()); //body mutex should be locked before call this method
		#endif

		workBody->setRestitution(restitution);
		workBody->setFriction(friction);
		workBody->setRollingFriction(rollingFriction);
		workBody->setCcdMotionThreshold(ccdMotionThreshold);
	}

	bool AbstractBody::applyFrom(const AbstractWorkBody *workBody)
	{
        #ifndef NDEBUG
            assert(!bodyMutex.try_lock()); //body mutex should be locked before call this method
        #endif

		bool fullRefreshRequested = bNeedFullRefresh.load(std::memory_order_relaxed);
		if(!fullRefreshRequested)
		{
			bIsActive.store(workBody->isActive(), std::memory_order_relaxed);

			transform.setPosition(workBody->getPosition());
			transform.setOrientation(workBody->getOrientation());
		}

		return fullRefreshRequested;
	}

	void AbstractBody::setTransform(const Transform<float> &transform)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		if(std::abs(transform.getScale()-this->transform.getScale()) > std::numeric_limits<float>::epsilon())
		{
			this->transform = transform;
			refreshScaledShape();
		}else
		{
			this->transform = transform;
		}

		this->setNeedFullRefresh(true);
		this->isManuallyMoved = true;
	}

	Transform<float> AbstractBody::getTransform() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return transform;
	}

	bool AbstractBody::isManuallyMovedAndResetFlag()
	{
		if(isManuallyMoved)
		{
			isManuallyMoved = false;
			return true;
		}
		return false;
	}

	void AbstractBody::setShape(const std::shared_ptr<const CollisionShape3D> &shape)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->originalShape = shape;
		refreshScaledShape();
		this->setNeedFullRefresh(true);
	}

	std::shared_ptr<const CollisionShape3D> AbstractBody::getOriginalShape() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return originalShape;
	}

	/**
	 * @return Scaled shape representing the form of the body. The shape is
	 * scaled according to scale define in 'transform' attribute.
	 */
	std::shared_ptr<const CollisionShape3D> AbstractBody::getScaledShape() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return scaledShape;
	}

	Vector3<float> AbstractBody::computeScaledShapeLocalInertia(float mass) const
	{
		return scaledShape->computeLocalInertia(mass);
	}

	void AbstractBody::setId(const std::string &id)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->id = id;
	}

	const std::string &AbstractBody::getId() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return id;
	}

	/**
	 * @param restitution Restitution (0=stop, 1=100% elastic)
	 */
	void AbstractBody::setRestitution(float restitution)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->restitution = restitution;
	}

	/**
	 * @return Restitution (0=stop, 1=100% elastic)
	 */
	float AbstractBody::getRestitution() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return restitution;
	}

	/**
	 * @param friction Friction (0=no friction, 1=total friction)
	 */
	void AbstractBody::setFriction(float friction)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->friction = friction;
	}

	/**
	 * @return Friction (0=no friction, 1=total friction)
	 */
	float AbstractBody::getFriction() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return friction;
	}

	/**
	 * @param rollingFriction Rolling friction (0=no friction, 1=total friction)
	 */
	void AbstractBody::setRollingFriction(float rollingFriction)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->rollingFriction = rollingFriction;
	}

	/**
	 * @return Rolling friction (0=no friction, 1=total friction)
	 */
	float AbstractBody::getRollingFriction() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return rollingFriction;
	}

	/**
	 * @return Threshold for continuous collision detection in distance unit. A default value is determinate automatically
	 * for each body thanks to properties 'collisionShape.ccdMotionThresholdFactor'.
	 */
	float AbstractBody::getCcdMotionThreshold() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return ccdMotionThreshold;
	}

	/**
	 * Process continuous collision detection if the motion in one step is more then threshold.
	 * @param ccdMotionThreshold Threshold for continuous collision detection in distance unit.
	 */
	void AbstractBody::setCcdMotionThreshold(float ccdMotionThreshold)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->ccdMotionThreshold = ccdMotionThreshold;
	}

	/**
	 * @return True when body is static (cannot be affected by physics world)
	 */
	bool AbstractBody::isStatic() const
	{
		return bIsStatic.load(std::memory_order_relaxed);
	}

	/**
	 * @param bIsStatic Indicate whether body is static (cannot be affected by physics world)
	 */
	void AbstractBody::setIsStatic(bool bIsStatic)
	{
		this->bIsStatic.store(bIsStatic, std::memory_order_relaxed);
	}

	/**
	 * @return True when body is active (body has velocity and/or one of body in same island is active)
	 */
	bool AbstractBody::isActive() const
	{
		return bIsActive.load(std::memory_order_relaxed);
	}

}
