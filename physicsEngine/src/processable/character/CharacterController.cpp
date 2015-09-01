#include <cassert>
#include <vector>
#include <limits>
#include <cmath>

#include "processable/character/CharacterController.h"
#include "collision/ManifoldResult.h"
#include "collision/ManifoldContactPoint.h"
#include "collision/broadphase/PairContainer.h"
#include "PhysicsWorld.h"

namespace urchin
{

	CharacterController::CharacterController(std::shared_ptr<const CollisionShape3D> characterShape, const PhysicsTransform &transform) :
		timeKeepMoveInAir(ConfigService::instance()->getFloatValue("character.timeKeepMoveInAir")),
		percentageControlInAir(ConfigService::instance()->getFloatValue("character.percentageControlInAir")),
		physicsWorld(nullptr),
		ghostBody(new WorkGhostBody("character", transform, characterShape)),
		verticalVelocity(0.0f),
		jumpSpeed(5.0f),
		makeJump(false),
		hasHit(false),
		numberOfHit(0),
		isOnGround(false),
		timeInTheAir(0.0f),
		slopeInPercentage(0.0f)
	{
		setMaxSlope(0.785398163f); //45 degrees
		ghostBody->setIsActive(true); //always active for get better reactivity
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
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		this->walkDirection = walkDirection;
	}

	const Vector3<float> &CharacterController::getWalkDirection() const
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		return walkDirection;
	}

	void CharacterController::setJumpSpeed(float jumpSpeed)
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		this->jumpSpeed = jumpSpeed;
	}

	float CharacterController::getJumpSpeed() const
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		return jumpSpeed;
	}

	void CharacterController::jump()
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		this->makeJump = true;
	}

	bool CharacterController::needJumpAndUpdateFlag()
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		if(makeJump)
		{
			makeJump = false;
			return true;
		}

		return false;
	}

	void CharacterController::setMaxSlope(float maxSlopeInRadian)
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		this->maxSlopeInRadian = maxSlopeInRadian;
		this->maxSlopeInPercentage = std::tan(maxSlopeInRadian);
	}

	float CharacterController::getMaxSlopeInRadian() const
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		return maxSlopeInRadian;
	}

	float CharacterController::getMaxSlopeInPercentage() const
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		return maxSlopeInPercentage;
	}

	void CharacterController::setTransform(const PhysicsTransform &ghostBodyTransform)
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

		this->ghostBodyTransform = ghostBodyTransform;
	}

	const PhysicsTransform &CharacterController::getTransform() const
	{
		boost::recursive_mutex::scoped_lock lock(characterMutex);

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
		if(needJumpAndUpdateFlag() && isOnGround)
		{
			verticalVelocity += getJumpSpeed();
			isOnGround = false;
		}

		//compute gravity velocity
		if(!isOnGround || numberOfHit > 1)
		{
			verticalVelocity -= (-gravity.Y) * dt;
			if(verticalVelocity < -55.0f)
			{
				verticalVelocity = -55.0f;
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
	void CharacterController::process(float dt, const Vector3<float> &gravity)
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

		//set new transform on character
		setTransform(ghostBody->getPhysicsTransform());
	}

	void CharacterController::recoverFromPenetration(float dt)
	{
		SignificantContactValues significantContactValues = resetSignificantContactValues();

		const std::vector<OverlappingPair *> &overlappingPairs = ghostBody->getPairContainer()->getOverlappingPairs();
		for(unsigned int subStepIndex=0; subStepIndex<RECOVER_PENETRATION_SUB_STEPS; ++subStepIndex)
		{
			std::vector<ManifoldResult> *manifoldResults = physicsWorld->getCollisionWorld()->getNarrowPhaseManager()->process(overlappingPairs);

			for(std::vector<ManifoldResult>::const_iterator it = manifoldResults->begin(); it!=manifoldResults->end(); ++it)
			{
				float sign = it->getBody1()==ghostBody ? -1.0 : 1.0;
				for(unsigned int i=0; i<it->getNumContactPoints(); ++i)
				{
					const ManifoldContactPoint &manifoldContactPoint = it->getManifoldContactPoint(i);
					float depth = manifoldContactPoint.getDepth();

					if(depth < 0.0005f)
					{
						Vector3<float> normal =  manifoldContactPoint.getNormalFromObject2() * sign;
						Vector3<float> moveVector = normal * depth * recoverFactors[subStepIndex];
						ghostBody->setPosition(ghostBody->getPosition().translate(moveVector));

						saveSignificantContactValues(significantContactValues, normal);
					}
				}
			}
		}

		computeSignificantContactValues(significantContactValues, dt);
	}

	SignificantContactValues CharacterController::resetSignificantContactValues()
	{
		SignificantContactValues significantContactValues;
		significantContactValues.hasHit = false;
		significantContactValues.nbOfRecover = 0;
		significantContactValues.maxDotProductUpNormalAxis = std::numeric_limits<float>::min();

		numberOfHit = 0;

		return significantContactValues;
	}

	void CharacterController::saveSignificantContactValues(SignificantContactValues &significantContactValues, const Vector3<float> &normal)
	{
		significantContactValues.hasHit = true;
		significantContactValues.nbOfRecover++;

		float dotProductUpNormalAxis = (-normal).dotProduct(Vector3<float>(0.0, 1.0, 0.0));
		if(dotProductUpNormalAxis > significantContactValues.maxDotProductUpNormalAxis)
		{
			significantContactValues.maxDotProductUpNormalAxis = dotProductUpNormalAxis;
			significantContactValues.mostUpVerticalNormal = -normal;
		}
	}

	void CharacterController::computeSignificantContactValues(SignificantContactValues &significantContactValues, float dt)
	{
		hasHit = significantContactValues.hasHit;
		numberOfHit = std::ceil(significantContactValues.nbOfRecover/RECOVER_PENETRATION_SUB_STEPS - 0.00001f);

		if(numberOfHit > 0)
		{
			isOnGround = std::acos(significantContactValues.maxDotProductUpNormalAxis) < getMaxSlopeInRadian();
		}else
		{
			isOnGround = false;
		}

		if(isOnGround)
		{
			timeInTheAir = 0.0f;
		}else
		{
			timeInTheAir += dt;
		}
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
