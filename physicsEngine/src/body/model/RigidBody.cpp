#include <stdexcept>
#include <limits>
#include <mutex>

#include "RigidBody.h"

namespace urchin
{

	RigidBody::RigidBody(const std::string &id, const Transform<float> &transform, const std::shared_ptr<const CollisionShape3D> &shape) :
			AbstractBody(id, transform, shape)
	{
		initializeRigidBody();
	}

	RigidBody::RigidBody(const RigidBody &rigidBody) :
		AbstractBody(rigidBody)
	{
		initializeRigidBody();

		setLinearFactor(rigidBody.getLinearFactor());
		setAngularFactor(rigidBody.getAngularFactor());
		setDamping(rigidBody.getLinearDamping(), rigidBody.getAngularDamping());
		setMass(rigidBody.getMass());
	}

	void RigidBody::initializeRigidBody()
	{
		//default values
		setMass(0.0f);
		linearDamping = 0.0f;
		angularDamping = 0.0f;
		linearFactor.setValues(1.0, 1.0, 1.0);
		angularFactor.setValues(1.0, 1.0, 1.0);
	}

	void RigidBody::refreshScaledShape()
	{
		AbstractBody::refreshScaledShape();

		refreshLocalInertia();
	}

	void RigidBody::refreshLocalInertia()
	{
		this->localInertia = computeScaledShapeLocalInertia(mass);
	}

	AbstractWorkBody *RigidBody::createWorkBody() const
	{
		const Transform<float> &transform = getTransform();
		PhysicsTransform physicsTransform(transform.getPosition(), transform.getOrientation());

		return new WorkRigidBody(getId(), physicsTransform, getScaledShape());
	}

	void RigidBody::updateTo(AbstractWorkBody *workBody)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		AbstractBody::updateTo(workBody);

		WorkRigidBody *workRigidBody = WorkRigidBody::upCast(workBody);
		if(workRigidBody)
		{
			workRigidBody->setTotalMomentum(totalMomentum);
			workRigidBody->setTotalTorqueMomentum(totalTorqueMomentum);
			workRigidBody->setMassProperties(mass, localInertia);
			workRigidBody->setDamping(linearDamping, angularDamping);
			workRigidBody->setLinearFactor(linearFactor);
			workRigidBody->setAngularFactor(angularFactor);

			workRigidBody->refreshInvWorldInertia();

			totalMomentum.setNull();
			totalTorqueMomentum.setNull();
		}
	}

	void RigidBody::applyFrom(const AbstractWorkBody *workBody)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		AbstractBody::applyFrom(workBody);

		const WorkRigidBody *workRigidBody = WorkRigidBody::upCast(workBody);
		if(workRigidBody)
		{
			linearVelocity = workRigidBody->getLinearVelocity();
			angularVelocity = workRigidBody->getAngularVelocity();
		}
	}

	Vector3<float> RigidBody::getLinearVelocity() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return linearVelocity;
	}

	Vector3<float> RigidBody::getAngularVelocity() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return angularVelocity;
	}

	Vector3<float> RigidBody::getTotalMomentum() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return totalMomentum;
	}

	void RigidBody::applyCentralMomentum(const Vector3<float> &momentum)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		totalMomentum += momentum;
	}

	void RigidBody::applyMomentum(const Vector3<float> &momentum, const Point3<float> &pos)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		//apply central force
		totalMomentum += momentum;

		//apply torque
		totalTorqueMomentum += pos.toVector().crossProduct(momentum);
	}

	Vector3<float> RigidBody::getTotalTorqueMomentum() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return totalTorqueMomentum;
	}

	void RigidBody::applyTorqueMomentum(const Vector3<float> &torqueMomentum)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		totalTorqueMomentum += torqueMomentum;
	}

	void RigidBody::setMass(float mass)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->mass = mass;
		refreshLocalInertia();

		setIsStatic(mass > -std::numeric_limits<float>::epsilon() && mass < std::numeric_limits<float>::epsilon());
	}

	float RigidBody::getMass() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return mass;
	}

	Vector3<float> RigidBody::getLocalInertia() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return localInertia;
	}

	/**
	 * Sets damping values. Values of damping must be between 0 and 1.
	 * Zero value means no resistance at all and value 1 stop directly the objects.
	 * Damping can be used to imitate air resistance.
	 */
	void RigidBody::setDamping(float linearDamping, float angularDamping)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

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
		std::lock_guard<std::mutex> lock(bodyMutex);

		return linearDamping;
	}

	float RigidBody::getAngularDamping() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return angularDamping;
	}

	/**
	 * @param linearFactor Linear factor. Linear factor allows to block movement if axis value is 0.
	 */
	void RigidBody::setLinearFactor(const Vector3<float> &linearFactor)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->linearFactor = linearFactor;
	}

	/**
	 * @return Linear factor. Linear factor allows to block movement if axis value is 0.
	 */
	Vector3<float> RigidBody::getLinearFactor() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return linearFactor;
	}

	/**
	 * @param angularFactor Angular factor. Angular factor allows to block rotation movement if axis value is 0.
	 */
	void RigidBody::setAngularFactor(const Vector3<float> &angularFactor)
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		this->angularFactor = angularFactor;
	}

	/**
	 * @return Angular factor. Angular factor allows to block rotation movement if axis value is 0.
	 */
	Vector3<float> RigidBody::getAngularFactor() const
	{
		std::lock_guard<std::mutex> lock(bodyMutex);

		return angularFactor;
	}

}
