#include <cassert>
#include <vector>
#include <limits>
#include <cmath>

#include "processable/character/CharacterController.h"
#include "collision/ManifoldResult.h"
#include "collision/ManifoldContactPoint.h"
#include "collision/broadphase/PairContainer.h"
#include "PhysicsWorld.h"

#define DEFAULT_MAX_SLOPE 3.14159f/4.0f //45 degrees
#define MIN_RECOVERABLE_DEPTH 0.0001f
#define MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND 0.15f;
#define MAX_VERTICAL_VELOCITY 55.0f

namespace urchin
{

	CharacterController::CharacterController(const std::shared_ptr<const CollisionShape3D> &characterShape, const PhysicsTransform &transform) :
		timeKeepMoveInAir(ConfigService::instance()->getFloatValue("character.timeKeepMoveInAir")),
		percentageControlInAir(ConfigService::instance()->getFloatValue("character.percentageControlInAir")),
		physicsWorld(nullptr),
		ghostBody(new WorkGhostBody("character", transform, characterShape)),
		verticalVelocity(0.0f),
		jumpSpeed(5.0f),
		makeJump(false),
		numberOfHit(0),
		isOnGround(false),
		hitRoof(false),
		timeInTheAir(0.0f),
		jumping(false),
		slopeInPercentage(0.0f)
	{
		setMaxSlope(DEFAULT_MAX_SLOPE);
		ghostBody->setIsActive(true); //always active for get better reactivity

		setTransform(ghostBody->getPhysicsTransform());
	}

	CharacterController::~CharacterController()
	{
		if(physicsWorld!=nullptr)
		{
			physicsWorld->getCollisionWorld()->getBroadPhaseManager()->removeBody(ghostBody);
		}

		delete ghostBody;
	}

	void CharacterController::initialize(PhysicsWorld *physicsWorld)
	{
		this->physicsWorld = physicsWorld;

		PairContainer *pairContainer = ghostBody->getPairContainer();
		physicsWorld->getCollisionWorld()->getBroadPhaseManager()->addBody(ghostBody, pairContainer);
	}

	void CharacterController::setWalkDirection(const Vector3<float> &walkDirection)
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		this->walkDirection = walkDirection;
	}

	const Vector3<float> &CharacterController::getWalkDirection() const
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		return walkDirection;
	}

	void CharacterController::setJumpSpeed(float jumpSpeed)
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		this->jumpSpeed = jumpSpeed;
	}

	float CharacterController::getJumpSpeed() const
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		return jumpSpeed;
	}

	void CharacterController::jump()
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		this->makeJump = true;
	}

	bool CharacterController::needJumpAndUpdateFlag()
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		if(makeJump)
		{
			makeJump = false;
			return true;
		}

		return false;
	}

	void CharacterController::setMaxSlope(float maxSlopeInRadian)
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		this->maxSlopeInRadian = maxSlopeInRadian;
		this->maxSlopeInPercentage = std::tan(maxSlopeInRadian);
	}

	float CharacterController::getMaxSlopeInRadian() const
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		return maxSlopeInRadian;
	}

	float CharacterController::getMaxSlopeInPercentage() const
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		return maxSlopeInPercentage;
	}

	void CharacterController::setTransform(const PhysicsTransform &ghostBodyTransform)
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		this->ghostBodyTransform = ghostBodyTransform;
	}

	const PhysicsTransform &CharacterController::getTransform() const
	{
		std::lock_guard<std::mutex> lock(characterMutex);

		return ghostBodyTransform;
	}

	/**
	 * @param dt Delta of time (sec.) between two simulation steps
	 * @param gravity Gravity expressed in units/s^2
	 */
	void CharacterController::setup(float dt, const Vector3<float> &gravity)
	{
		#ifdef _DEBUG
			assert(physicsWorld!=nullptr);
		#endif

		//save values
		previousBodyPosition = ghostBody->getPosition();

		//apply user move
		Point3<float> targetPosition = ghostBody->getPosition();
		if(isOnGround)
		{
			Vector3<float> walkDirection = getWalkDirection();
			float slopeSpeedDecrease = MathAlgorithm::clamp(1.0f - (slopeInPercentage / getMaxSlopeInPercentage()), 0.0f, 1.0f);
			targetPosition = targetPosition.translate(walkDirection * slopeSpeedDecrease);

			lastWalkDirection = walkDirection;
		}else if(timeInTheAir < timeKeepMoveInAir)
		{
			float momentumSpeedDecrease = 1.0f - (timeInTheAir / (float)timeKeepMoveInAir);
			Vector3<float> walkDirectionInAir = lastWalkDirection*(1.0f-percentageControlInAir)
					+ walkDirection*percentageControlInAir;
			targetPosition = targetPosition.translate(walkDirectionInAir * momentumSpeedDecrease);
		}else
		{
			lastWalkDirection.X = lastWalkDirection.Y = lastWalkDirection.Z = 0.0f;
		}

		//jump
		bool closeToTheGround = timeInTheAir < MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND;
		if(needJumpAndUpdateFlag() && closeToTheGround && !jumping)
		{
			verticalVelocity += getJumpSpeed();
			isOnGround = false;
			jumping = true;
		}else if(isOnGround && jumping){
			jumping = false;
		}

		//compute gravity velocity
		if(!isOnGround || numberOfHit > 1)
		{
			verticalVelocity -= (-gravity.Y) * dt;
			if(verticalVelocity < -MAX_VERTICAL_VELOCITY)
			{
				verticalVelocity = -MAX_VERTICAL_VELOCITY;
			}
		}

		//apply data on body
		targetPosition.Y += verticalVelocity * dt;
		ghostBody->setPosition(targetPosition);
	}

	/**
	 * @param dt Delta of time between two simulation steps
	 * @param gravity Gravity expressed in units/s^2
	 */
	void CharacterController::execute(float dt, const Vector3<float> &gravity)
	{
		//recover from penetration
		recoverFromPenetration(dt);

		//compute values
		slopeInPercentage = 0.0f;
		if(isOnGround)
		{
			verticalVelocity = 0.0f;
			slopeInPercentage = computeSlope();
		}
		if(hitRoof)
		{
			verticalVelocity = 0.0f;
		}

		//set new transform on character
		setTransform(ghostBody->getPhysicsTransform());
	}

	void CharacterController::recoverFromPenetration(float dt)
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

	SignificantContactValues CharacterController::resetSignificantContactValues()
	{
		SignificantContactValues significantContactValues;
		significantContactValues.numberOfHit = 0;

		significantContactValues.maxDotProductUpNormalAxis = std::numeric_limits<float>::min();
		significantContactValues.maxDotProductDownNormalAxis = -std::numeric_limits<float>::max();

		return significantContactValues;
	}

	void CharacterController::saveSignificantContactValues(SignificantContactValues &significantContactValues, const Vector3<float> &normal)
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

	void CharacterController::computeSignificantContactValues(SignificantContactValues &significantContactValues, float dt)
	{
		numberOfHit = significantContactValues.numberOfHit;
		isOnGround = numberOfHit>0 && std::acos(significantContactValues.maxDotProductUpNormalAxis) < getMaxSlopeInRadian();
		hitRoof = numberOfHit>0 && std::acos(significantContactValues.maxDotProductDownNormalAxis) < getMaxSlopeInRadian();
		timeInTheAir = isOnGround ? 0.0f : timeInTheAir+dt;
	}

	/**
	 * Compute slope based on previous body position.
	 * Slope is expressed in percentage. A positive value means that character climb.
	 */
	float CharacterController::computeSlope()
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
