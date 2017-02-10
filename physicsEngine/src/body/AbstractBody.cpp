#include <stdexcept>
#include <limits>

#include "body/AbstractBody.h"

namespace urchin
{

	AbstractBody::AbstractBody(const std::string &id, const Transform<float> &transform, const std::shared_ptr<const CollisionShape3D> &shape) :
			ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")),
			workBody(nullptr),
			transform(transform),
			id(id),
			originalShape(shape)
	{
		//technical data
		bIsNew.store(false, std::memory_order_relaxed);
		bIsDeleted.store(false, std::memory_order_relaxed);
		bNeedFullRefresh.store(false, std::memory_order_relaxed);
		bIsStatic.store(true, std::memory_order_relaxed);
		bIsActive.store(false, std::memory_order_relaxed);

		//body description data
		refreshScaledShape();
		restitution = 0.2f;
		friction = 0.5f;
		rollingFriction = 0.0f;
	}

	AbstractBody::~AbstractBody()
	{

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
		#ifdef _DEBUG
			if(bodyMutex.try_lock())
			{
				throw std::runtime_error("Body mutex should be locked before call this method.");
			}
		#endif

		workBody->setRestitution(restitution);
		workBody->setFriction(friction);
		workBody->setRollingFriction(rollingFriction);
		workBody->setCcdMotionThreshold(ccdMotionThreshold);
	}

	void AbstractBody::applyFrom(const AbstractWorkBody *workBody)
	{
		#ifdef _DEBUG
			if(bodyMutex.try_lock())
			{
				throw std::runtime_error("Body mutex should be locked before call this method.");
			}
		#endif

		if(true||!bNeedFullRefresh.load(std::memory_order_relaxed))
		{
			bIsActive.store(workBody->isActive(), std::memory_order_relaxed);

			transform.setPosition(workBody->getPosition());
			transform.setOrientation(workBody->getOrientation());
		}
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
	}

	const Transform<float> &AbstractBody::getTransform() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return transform;
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
		#ifdef _DEBUG
			if(bodyMutex.try_lock())
			{
				throw std::runtime_error("Body mutex should be locked before call this method.");
			}
		#endif

		return scaledShape->computeLocalInertia(mass);
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
