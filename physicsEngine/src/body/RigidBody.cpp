#include <stdexcept>
#include <limits>
#include <boost/thread.hpp>

#include "body/RigidBody.h"

namespace urchin
{

	RigidBody::RigidBody(const std::string &id, const Transform<float> &transform, std::shared_ptr<const CollisionShape3D> shape) :
			AbstractBody(id, transform, shape)
	{
		//default values
		mass = 0.0f;
		linearDamping = 0.0f;
		angularDamping = 0.0f;
		linearFactor.setValues(1.0, 1.0, 1.0);
		angularFactor.setValues(1.0, 1.0, 1.0);
	}

	RigidBody::~RigidBody()
	{

	}

	AbstractWorkBody *RigidBody::createWorkBody() const
	{
		const Transform<float> &transform = getTransform();
		PhysicsTransform physicsTransform(transform.getPosition(), transform.getOrientation());

		return new WorkRigidBody(getId(), physicsTransform, getScaledShape());
	}

	void RigidBody::update(AbstractWorkBody *workBody)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		AbstractBody::update(workBody);

		WorkRigidBody *workRigidBody = WorkRigidBody::upCast(workBody);
		if(workRigidBody!=nullptr)
		{
			workRigidBody->setTotalForce(totalForce);
			workRigidBody->setTotalTorque(totalTorque);
			workRigidBody->setMassProperties(mass, localInertia);
			workRigidBody->setDamping(linearDamping, angularDamping);
			workRigidBody->setLinearFactor(linearFactor);
			workRigidBody->setAngularFactor(angularFactor);

			workRigidBody->refreshInvWorldInertia();

			totalForce.setNull();
			totalTorque.setNull();
		}
	}

	void RigidBody::apply(const AbstractWorkBody *workBody)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		AbstractBody::apply(workBody);

		const WorkRigidBody *workRigidBody = WorkRigidBody::upCast(workBody);
		if(workRigidBody!=nullptr)
		{
			linearVelocity = workRigidBody->getLinearVelocity();
			angularVelocity = workRigidBody->getAngularVelocity();
		}
	}

	const Vector3<float> &RigidBody::getLinearVelocity() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return linearVelocity;
	}

	const Vector3<float> &RigidBody::getAngularVelocity() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return angularVelocity;
	}

	const Vector3<float> &RigidBody::getTotalForce() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return totalForce;
	}

	void RigidBody::applyCentralForce(const Vector3<float> &force)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		totalForce += force;
	}

	void RigidBody::applyForce(const Vector3<float> &force, const Point3<float> &pos)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		//apply central force
		totalForce += force;

		//apply torque
		totalTorque += pos.toVector().crossProduct(force);
	}

	const Vector3<float> &RigidBody::getTotalTorque() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return totalTorque;
	}

	void RigidBody::applyTorque(const Vector3<float> &torque)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		totalTorque += torque;
	}

	void RigidBody::setMassProperties(float mass, const Vector3<float> &localInertia)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		this->mass = mass;
		this->localInertia = localInertia;

		setIsStatic(mass > -std::numeric_limits<float>::epsilon() && mass < std::numeric_limits<float>::epsilon());
	}

	float RigidBody::getMass() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return mass;
	}

	const Vector3<float> &RigidBody::getLocalInertia() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return localInertia;
	}

	/**
	 * Sets damping values. Values of damping must be between 0 and 1.
	 * Zero value means no resistance at all and value 1 stop directly the objects.
	 * Damping can be used to imitate air resistance.
	 */
	void RigidBody::setDamping(float linearDamping, float angularDamping)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		if(linearDamping < 0.0 || linearDamping > 1.0)
		{
			throw std::domain_error("Wrong linear damping value.");
		}

		if(angularDamping < 0.0 || angularDamping > 1.0)
		{
			throw std::domain_error("Wrong angular damping value.");
		}

		this->linearDamping = linearDamping;
		this->angularDamping = angularDamping;
	}

	float RigidBody::getLinearDamping() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return linearDamping;
	}

	float RigidBody::getAngularDamping() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return angularDamping;
	}

	/**
	 * @param linearFactor Linear factor. Linear factor allows to block movement if axis value is 0.
	 */
	void RigidBody::setLinearFactor(const Vector3<float> &linearFactor)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		this->linearFactor = linearFactor;
	}

	/**
	 * @return Linear factor. Linear factor allows to block movement if axis value is 0.
	 */
	const Vector3<float> &RigidBody::getLinearFactor() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return linearFactor;
	}

	/**
	 * @param angularFactor Angular factor. Angular factor allows to block rotation movement if axis value is 0.
	 */
	void RigidBody::setAngularFactor(const Vector3<float> &angularFactor)
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		this->angularFactor = angularFactor;
	}

	/**
	 * @return Angular factor. Angular factor allows to block rotation movement if axis value is 0.
	 */
	const Vector3<float> &RigidBody::getAngularFactor() const
	{
		boost::recursive_mutex::scoped_lock lock(bodyMutex);

		return angularFactor;
	}

}
