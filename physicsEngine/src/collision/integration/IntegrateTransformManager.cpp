#include "collision/integration/IntegrateTransformManager.h"

namespace urchin
{

	IntegrateTransformManager::IntegrateTransformManager(const BodyManager *bodyManager) :
			bodyManager(bodyManager)
	{

	}

	IntegrateTransformManager::~IntegrateTransformManager()
	{

	}

	/**
	 * @param dt Delta of time between two simulation steps
	 */
	void IntegrateTransformManager::integrateTransform(float dt)
	{
		for(unsigned int i=0; i<bodyManager->getWorkBodies().size(); ++i)
		{
			WorkRigidBody *body = WorkRigidBody::upCast(bodyManager->getWorkBodies()[i]);
			if(body!=nullptr && body->isActive())
			{
				//update position
				body->setPosition(body->getPosition().translate(body->getLinearVelocity() * dt));

				//update orientation
				float length = body->getAngularVelocity().length();
				if(length > 0.0)
				{
					const Vector3<float> normalizedAxis = body->getAngularVelocity() / length;
					const float angle = length * dt;

					Quaternion<float> newOrientation = Quaternion<float>(normalizedAxis, angle) * body->getOrientation();
					newOrientation = newOrientation.normalize();
					body->setOrientation(newOrientation);
				}
			}
		}
	}

}
