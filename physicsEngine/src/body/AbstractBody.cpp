#include "body/AbstractBody.h"

namespace urchin
{

	AbstractBody::AbstractBody(const std::string &id, const Transform<float> &transform, std::shared_ptr<const CollisionShape3D> shape) :
			workBody(nullptr),
			transform(transform),
			originalShape(shape),
			id(id)
	{
		this->scaledShape = originalShape->scale(transform.getScale());

		//default values
		restitution = 0.2f;
		friction = 0.5f;
		rollingFriction = 0.0f;

		bIsNew.store(false, std::memory_order_relaxed);
		bIsDeleted.store(false, std::memory_order_relaxed);
		bIsStatic.store(true, std::memory_order_relaxed);
		bIsActive.store(false, std::memory_order_relaxed);
	}

	AbstractBody::~AbstractBody()
	{

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

	void AbstractBody::setWorkBody(AbstractWorkBody *workBody)
	{
		this->workBody = workBody;
	}

	AbstractWorkBody *AbstractBody::getWorkBody() const
	{
		return workBody;
	}

	void AbstractBody::update(AbstractWorkBody *workBody)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		workBody->setRestitution(restitution);
		workBody->setFriction(friction);
		workBody->setRollingFriction(rollingFriction);
	}

	void AbstractBody::apply(const AbstractWorkBody *workBody)
	{
		{
			boost::recursive_mutex::scoped_lock lock(bodyMutex);

			transform.setPosition(workBody->getPosition());
			transform.setOrientation(workBody->getOrientation());
		}
		bIsActive.store(workBody->isActive(), std::memory_order_relaxed);
	}

	const Transform<float> &AbstractBody::getTransform() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return transform;
	}

	const Point3<float> &AbstractBody::getPosition() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return transform.getPosition();
	}

	const Quaternion<float> &AbstractBody::getOrientation() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return transform.getOrientation();
	}

	std::shared_ptr<const CollisionShape3D> AbstractBody::getOriginalShape() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return originalShape;
	}

	/**
	 * @return Scaled shape representing the form of the body. The shape is
	 * scaled according to scale define in 'transform' attribute.
	 */
	std::shared_ptr<const CollisionShape3D> AbstractBody::getScaledShape() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return scaledShape;
	}

	const std::string &AbstractBody::getId() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return id;
	}

	/**
	 * @param restitution Restitution (0=stop, 1=100% elastic)
	 */
	void AbstractBody::setRestitution(float restitution)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		this->restitution = restitution;
	}

	/**
	 * @return Restitution (0=stop, 1=100% elastic)
	 */
	float AbstractBody::getRestitution() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return restitution;
	}

	/**
	 * @param friction Friction (0=no friction, 1=total friction)
	 */
	void AbstractBody::setFriction(float friction)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		this->friction = friction;
	}

	/**
	 * @return Friction (0=no friction, 1=total friction)
	 */
	float AbstractBody::getFriction() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return friction;
	}

	/**
	 * @param rollingFriction Rolling friction (0=no friction, 1=total friction)
	 */
	void AbstractBody::setRollingFriction(float rollingFriction)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		this->rollingFriction = rollingFriction;
	}

	/**
	 * @return Rolling friction (0=no friction, 1=total friction)
	 */
	float AbstractBody::getRollingFriction() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return rollingFriction;
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
