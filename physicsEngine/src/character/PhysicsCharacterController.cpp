#include <cassert>
#include <vector>
#include <limits>
#include <cmath>

#include "PhysicsCharacterController.h"
#include "collision/ManifoldContactPoint.h"
#include "PhysicsWorld.h"

#define MIN_RECOVERABLE_DEPTH 0.0001f
#define MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND 0.15f
#define MAX_VERTICAL_SPEED 55.0f

namespace urchin
{

	PhysicsCharacterController::PhysicsCharacterController(const std::shared_ptr<PhysicsCharacter> &physicsCharacter, PhysicsWorld *physicsWorld) :
		timeKeepMoveInAir(ConfigService::instance()->getFloatValue("character.timeKeepMoveInAir")),
		percentageControlInAir(ConfigService::instance()->getFloatValue("character.percentageControlInAir")),
        physicsCharacter(physicsCharacter),
        physicsWorld(physicsWorld),
		ghostBody(new WorkGhostBody(physicsCharacter->getName(), physicsCharacter->getTransform(), physicsCharacter->getShape())),
		verticalSpeed(0.0f),
		makeJump(false),
        initialOrientation(physicsCharacter->getTransform().getOrientation()),
		numberOfHit(0),
		isOnGround(false),
		hitRoof(false),
		timeInTheAir(0.0f),
		jumping(false),
		slopeInPercentage(0.0f)
	{
		#ifdef _DEBUG
			assert(physicsWorld!=nullptr);
        #endif

		ghostBody->setIsActive(true); //always active for get better reactivity
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->addBodyAsync(ghostBody);
	}

	PhysicsCharacterController::~PhysicsCharacterController()
	{
	    physicsWorld->getCollisionWorld()->getBroadPhaseManager()->removeBodyAsync(ghostBody);
	}

	void PhysicsCharacterController::setMomentum(const Vector3<float> &momentum)
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		this->velocity = (momentum / physicsCharacter->getMass());
	}

	Vector3<float> PhysicsCharacterController::getVelocity() const
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		return velocity;
	}

	void PhysicsCharacterController::jump()
	{
		makeJump.store(true, std::memory_order_relaxed);
	}

	bool PhysicsCharacterController::needJumpAndResetFlag()
	{
		return makeJump.exchange(false, std::memory_order_relaxed);
	}

	/**
	 * @param dt Delta of time between two simulation steps
	 */
	void PhysicsCharacterController::update(float dt)
	{
		ScopeProfiler profiler("physics", "charactCtrlExec");

		//setup values
		setup(dt);

		//recover from penetration
		recoverFromPenetration(dt);

		//compute values
		slopeInPercentage = 0.0f;
		if(isOnGround)
		{
			verticalSpeed = 0.0f;
			slopeInPercentage = computeSlope();
		}
		if(hitRoof)
		{
			verticalSpeed = 0.0f;
		}

		//set new transform on character
        physicsCharacter->updateTransform(ghostBody->getPhysicsTransform());
	}

	void PhysicsCharacterController::setup(float dt)
	{
		//save values
		previousBodyPosition = ghostBody->getPosition();

		//apply user move
		Point3<float> targetPosition = ghostBody->getPosition();
		Vector3<float> velocity = getVelocity();
		if(isOnGround)
		{
			float slopeSpeedDecrease = 1.0f - (slopeInPercentage / physicsCharacter->getMaxSlopeInPercentage());
			slopeSpeedDecrease = MathAlgorithm::clamp(slopeSpeedDecrease, MIN_WALK_SPEED_PERCENTAGE, MAX_WALK_SPEED_PERCENTAGE);
			targetPosition = targetPosition.translate(velocity * dt * slopeSpeedDecrease);

			lastVelocity = velocity;
		}else if(timeInTheAir < timeKeepMoveInAir)
		{
			float momentumSpeedDecrease = 1.0f - (timeInTheAir / timeKeepMoveInAir);
			Vector3<float> walkDirectionInAir = velocity * (1.0f - percentageControlInAir) + velocity * percentageControlInAir;
			targetPosition = targetPosition.translate(walkDirectionInAir * dt * momentumSpeedDecrease);
		}else
		{
			lastVelocity.setNull();
		}

		//jump
		bool closeToTheGround = timeInTheAir < MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND;
		if(needJumpAndResetFlag() && closeToTheGround && !jumping)
		{
			verticalSpeed += physicsCharacter->getJumpSpeed();
			isOnGround = false;
			jumping = true;
		}else if(isOnGround && jumping)
		{
			jumping = false;
		}

		//compute gravity velocity
		if(!isOnGround || numberOfHit > 1)
		{
			verticalSpeed -= (-physicsWorld->getGravity().Y) * dt;
			if(verticalSpeed < -MAX_VERTICAL_SPEED)
			{
				verticalSpeed = -MAX_VERTICAL_SPEED;
			}
		}

		//compute and apply orientation
		if(!MathAlgorithm::isZero(velocity.squareLength(), 0.001f))
        {
            Quaternion<float> orientation = Quaternion<float>(velocity.normalize()).normalize();
            ghostBody->setOrientation(orientation * initialOrientation);
		}

		//apply data on body
		targetPosition.Y += verticalSpeed * dt;
		ghostBody->setPosition(targetPosition);
	}

	void PhysicsCharacterController::recoverFromPenetration(float dt)
	{
		SignificantContactValues significantContactValues = resetSignificantContactValues();

		for(unsigned int subStepIndex=0; subStepIndex<RECOVER_PENETRATION_SUB_STEPS; ++subStepIndex)
		{
			manifoldResults.clear();
			physicsWorld->getCollisionWorld()->getNarrowPhaseManager()->processGhostBody(ghostBody, manifoldResults);

			for(const auto &manifoldResult : manifoldResults)
			{
				float sign = manifoldResult.getBody1()==ghostBody ? -1.0 : 1.0;
				for(unsigned int i=0; i<manifoldResult.getNumContactPoints(); ++i)
				{
					const ManifoldContactPoint &manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
					float depth = manifoldContactPoint.getDepth();

					if(depth < MIN_RECOVERABLE_DEPTH)
					{
						Vector3<float> normal =  manifoldContactPoint.getNormalFromObject2() * sign;
						Vector3<float> moveVector = normal * depth * recoverFactors[subStepIndex];

						ghostBody->setPosition(ghostBody->getPosition().translate(moveVector));

						if(subStepIndex==0)
						{
							saveSignificantContactValues(significantContactValues, normal);
						}
					}
				}
			}
		}

		computeSignificantContactValues(significantContactValues, dt);
	}

	SignificantContactValues PhysicsCharacterController::resetSignificantContactValues()
	{
		SignificantContactValues significantContactValues;
		significantContactValues.numberOfHit = 0;

		significantContactValues.maxDotProductUpNormalAxis = std::numeric_limits<float>::min();
		significantContactValues.maxDotProductDownNormalAxis = -std::numeric_limits<float>::max();

		return significantContactValues;
	}

	void PhysicsCharacterController::saveSignificantContactValues(SignificantContactValues &significantContactValues, const Vector3<float> &normal)
	{
		significantContactValues.numberOfHit++;

		float dotProductUpNormalAxis = (-normal).dotProduct(Vector3<float>(0.0, 1.0, 0.0));
		if(dotProductUpNormalAxis > significantContactValues.maxDotProductUpNormalAxis)
		{
			significantContactValues.maxDotProductUpNormalAxis = dotProductUpNormalAxis;
			significantContactValues.mostUpVerticalNormal = -normal;
		}

		float dotProductDownNormalAxis = (-normal).dotProduct(Vector3<float>(0.0, -1.0, 0.0));
		if(dotProductDownNormalAxis > significantContactValues.maxDotProductDownNormalAxis)
		{
			significantContactValues.maxDotProductDownNormalAxis = dotProductDownNormalAxis;
			significantContactValues.mostDownVerticalNormal = -normal;
		}
	}

	void PhysicsCharacterController::computeSignificantContactValues(SignificantContactValues &significantContactValues, float dt)
	{
		numberOfHit = significantContactValues.numberOfHit;
		isOnGround = numberOfHit > 0 && std::acos(significantContactValues.maxDotProductUpNormalAxis) < physicsCharacter->getMaxSlopeInRadian();
		hitRoof = numberOfHit > 0 && std::acos(significantContactValues.maxDotProductDownNormalAxis) < physicsCharacter->getMaxSlopeInRadian();
		timeInTheAir = isOnGround ? 0.0f : timeInTheAir+dt;
	}

	/**
	 * Compute slope based on previous body position.
	 * Slope is expressed in percentage. A positive value means that character climb.
	 */
	float PhysicsCharacterController::computeSlope()
	{
		Point2<float> p1 = Point2<float>(ghostBody->getPosition().X, ghostBody->getPosition().Z);
		Point2<float> p2 = Point2<float>(previousBodyPosition.X, previousBodyPosition.Z);
		float run = p1.vector(p2).length();
		if(run==0.0f)
		{
			return 0.0f;
		}

		float rise = ghostBody->getPosition().Y - previousBodyPosition.Y;

		return rise / run;
	}
}
