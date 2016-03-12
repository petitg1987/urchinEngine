#include <limits>

#include "body/work/WorkRigidBody.h"
#include "body/InertiaCalculation.h"

namespace urchin
{

	WorkRigidBody::WorkRigidBody(const std::string &id, const PhysicsTransform &physicsTransform, const std::shared_ptr<const CollisionShape3D> &shape) :
			AbstractWorkBody(id, physicsTransform, shape),
			mass(0.0f),
			invMass(0.0f),
			invLocalInertia(Vector3<float>(0.0, 0.0, 0.0)),
			invWorldInertia(Matrix3<float>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)),
			linearDamping(0.0),
			angularDamping(0.0)
	{

	}

	WorkRigidBody::~WorkRigidBody()
	{

	}

	WorkRigidBody *WorkRigidBody::upCast(AbstractWorkBody *workBody)
	{
		return dynamic_cast<WorkRigidBody*>(workBody);
	}

	const WorkRigidBody *WorkRigidBody::upCast(const AbstractWorkBody *workBody)
	{
		return dynamic_cast<const WorkRigidBody*>(workBody);
	}

	/**
	 * Refresh body active state. If forces are apply on body: active body
	 */
	void WorkRigidBody::refreshBodyActiveState()
	{
		if(!isStatic() && !isActive())
		{
			if(totalForce.squareLength() > std::numeric_limits<float>::epsilon()
					|| totalTorque.squareLength() > std::numeric_limits<float>::epsilon())
			{
				setIsActive(true);
			}
		}
	}

	void WorkRigidBody::setLinearVelocity(const Vector3<float> &linearVelocity)
	{
		this->linearVelocity = linearVelocity;
	}

	const Vector3<float> &WorkRigidBody::getLinearVelocity() const
	{
		return linearVelocity;
	}

	void WorkRigidBody::setAngularVelocity(const Vector3<float> &angularVelocity)
	{
		this->angularVelocity = angularVelocity;
	}

	const Vector3<float> &WorkRigidBody::getAngularVelocity() const
	{
		return angularVelocity;
	}

	const Vector3<float> &WorkRigidBody::getTotalForce() const
	{
		return totalForce;
	}

	void WorkRigidBody::setTotalForce(const Vector3<float> &totalForce)
	{
		this->totalForce = totalForce;

		refreshBodyActiveState();
	}

	void WorkRigidBody::applyCentralForce(const Vector3<float> &force)
	{
		totalForce += force * linearFactor;
	}

	void WorkRigidBody::applyForce(const Vector3<float> &force, const Point3<float> &forcePoint)
	{
		//apply central force
		totalForce += force * linearFactor;

		//apply torque
		totalTorque += forcePoint.toVector().crossProduct(force * linearFactor);
	}

	void WorkRigidBody::resetForce()
	{
		totalForce.setValues(0.0, 0.0, 0.0);
	}

	const Vector3<float> &WorkRigidBody::getTotalTorque() const
	{
		return totalTorque;
	}

	void WorkRigidBody::setTotalTorque(const Vector3<float> &totalTorque)
	{
		this->totalTorque = totalTorque;

		refreshBodyActiveState();
	}

	void WorkRigidBody::applyTorque(const Vector3<float> &torque)
	{
		totalTorque += torque * angularFactor;
	}

	void WorkRigidBody::resetTorque()
	{
		totalTorque.setValues(0.0, 0.0, 0.0);
	}

	void WorkRigidBody::setMassProperties(float mass, const Vector3<float> &localInertia)
	{
		if(MathAlgorithm::isZero(mass))
		{
			setIsStatic(true);
			setIsActive(false);
			this->invMass = 0.0f;
		}else
		{
			setIsStatic(false);
			setIsActive(true);
			this->invMass = 1.0f / mass;
		}
		this->mass = mass;

		this->invLocalInertia.X = MathAlgorithm::isZero(localInertia.X) ? 0.0 : 1.0/localInertia.X;
		this->invLocalInertia.Y = MathAlgorithm::isZero(localInertia.Y) ? 0.0 : 1.0/localInertia.Y;
		this->invLocalInertia.Z = MathAlgorithm::isZero(localInertia.Z) ? 0.0 : 1.0/localInertia.Z;
	}

	float WorkRigidBody::getMass() const
	{
		return mass;
	}

	float WorkRigidBody::getInvMass() const
	{
		return invMass;
	}

	/**
	 * Refresh inverse world inertia based on body transformation
	 */
	void WorkRigidBody::refreshInvWorldInertia()
	{
		if(!isStatic())
		{
			invWorldInertia = InertiaCalculation::computeInverseWorldInertia(invLocalInertia, getPhysicsTransform());
		}else
		{
			invWorldInertia = Matrix3<float>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}
	}

	const Vector3<float> &WorkRigidBody::getInvLocalInertia() const
	{
		return invLocalInertia;
	}

	const Matrix3<float> &WorkRigidBody::getInvWorldInertia() const
	{
		return invWorldInertia;
	}

	/**
	 * Sets damping values. Values of damping must be between 0 and 1.
	 * Zero value means no resistance at all and value 1 stop directly the objects.
	 */
	void WorkRigidBody::setDamping(float linearDamping, float angularDamping)
	{
		this->linearDamping = linearDamping;
		this->angularDamping = angularDamping;
	}

	float WorkRigidBody::getLinearDamping() const
	{
		return linearDamping;
	}

	float WorkRigidBody::getAngularDamping() const
	{
		return angularDamping;
	}

	/**
	 * @param linearFactor Linear factor. Linear factor allows to block movement if axis value is 0.
	 */
	void WorkRigidBody::setLinearFactor(const Vector3<float> &linearFactor)
	{
		this->linearFactor = linearFactor;
	}

	/**
	 * @return Linear factor. Linear factor allows to block movement if axis value is 0.
	 */
	const Vector3<float> &WorkRigidBody::getLinearFactor() const
	{
		return linearFactor;
	}

	/**
	 * @param angularFactor Angular factor. Angular factor allows to block rotation movement if axis value is 0.
	 */
	void WorkRigidBody::setAngularFactor(const Vector3<float> &angularFactor)
	{
		this->angularFactor = angularFactor;
	}

	/**
	 * @return Angular factor. Angular factor allows to block rotation movement if axis value is 0.
	 */
	const Vector3<float> &WorkRigidBody::getAngularFactor() const
	{
		return angularFactor;
	}

}
