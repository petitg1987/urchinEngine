#include "collision/integration/IntegrateVelocityManager.h"
#include "body/work/WorkRigidBody.h"

namespace urchin
{

	IntegrateVelocityManager::IntegrateVelocityManager(const BodyManager *bodyManager) :
		bodyManager(bodyManager)
	{

	}

	/**
	 * @param dt Delta of time (sec.) between two simulation steps
	 * @param manifoldResults Constraints to solve
	 * @param gravity Gravity expressed in units/s^2
	 */
	void IntegrateVelocityManager::integrateVelocity(float dt, std::vector<ManifoldResult> &manifoldResult, const Vector3<float> &gravity)
	{
		//apply internal forces
		applyGravityForce(gravity);
		applyRollingFrictionResistanceForce(dt, manifoldResult);

		//integrate velocities and apply damping
		for (auto abstractBody : bodyManager->getWorkBodies())
		{
			WorkRigidBody *body = WorkRigidBody::upCast(abstractBody);
			if(body!=nullptr && body->isActive())
			{
				//integrate velocity
				body->setLinearVelocity(body->getLinearVelocity() + (body->getTotalForce() * body->getInvMass()) * dt);
				body->setAngularVelocity(body->getAngularVelocity() + (body->getTotalTorque() * body->getInvWorldInertia()) * dt);

				//apply damping
				body->setLinearVelocity(body->getLinearVelocity() * powf(1.0-body->getLinearDamping(), dt));
				body->setAngularVelocity(body->getAngularVelocity() * powf(1.0-body->getAngularDamping(), dt));

				//reset forces
				body->resetForce();
				body->resetTorque();
			}
		}
	}

	/**
	 * @param gravity Gravity expressed in units/s^2
	 */
	void IntegrateVelocityManager::applyGravityForce(const Vector3<float> &gravity)
	{
		for (auto abstractBody : bodyManager->getWorkBodies())
		{
			WorkRigidBody *body = WorkRigidBody::upCast(abstractBody);
			if(body!=nullptr && body->isActive())
			{
				body->applyCentralForce(gravity * body->getMass());
			}
		}
	}

	/**
	 * @param dt Delta of time (sec.) between two simulation steps
	 * @param manifoldResults Constraints to solve
	 */
	void IntegrateVelocityManager::applyRollingFrictionResistanceForce(float dt, std::vector<ManifoldResult> &manifoldResults)
	{
		for (const auto &result : manifoldResults)
		{
			float rollingFriction = std::max(result.getBody1()->getRollingFriction(), result.getBody2()->getRollingFriction());

			for(unsigned int bodyIndex=0; bodyIndex<2; ++bodyIndex)
			{
				WorkRigidBody *body =  WorkRigidBody::upCast(result.getBody(bodyIndex));
				if(body!=nullptr && body->isActive())
				{
					Matrix3<float> inertia = body->getInvWorldInertia().inverse();
					Vector3<float> currentTorqueForce = (body->getAngularVelocity() * inertia) / dt;

					Vector3<float> rollingFrictionForceDirection = -currentTorqueForce.normalize();
					Vector3<float> rollingFrictionForce = rollingFrictionForceDirection * rollingFriction * body->getMass();

					for(unsigned int i=0; i<3; ++i)
					{
						if(fabs(rollingFrictionForce[i]) > fabs(currentTorqueForce[i]))
						{
							//If rolling resistance force is higher that torque force: we apply negative torque force in order to stop rolling.
							//By doing that, we imitate a static friction.
							rollingFrictionForce[i] = -currentTorqueForce[i];
						}
					}

					body->applyTorque(rollingFrictionForce);
				}
			}
		}
	}

}
