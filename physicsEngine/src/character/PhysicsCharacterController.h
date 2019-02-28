#ifndef URCHINENGINE_PHYSICSCHARATERCONTROLLER_H
#define URCHINENGINE_PHYSICSCHARATERCONTROLLER_H

#include <memory>
#include <mutex>
#include <atomic>

#include "PhysicsCharacter.h"
#include "processable/Processable.h"
#include "collision/ManifoldResult.h"

#define RECOVER_PENETRATION_SUB_STEPS 4 //number of steps to recover character from penetration
#define MIN_WALK_SPEED_PERCENTAGE 0.75f
#define MAX_WALK_SPEED_PERCENTAGE 1.25f

namespace urchin
{

	struct SignificantContactValues
	{
		unsigned int numberOfHit;

		float maxDotProductUpNormalAxis;
		Vector3<float> mostUpVerticalNormal;

		float maxDotProductDownNormalAxis;
		Vector3<float> mostDownVerticalNormal;
	};

	/**
	* Character controller: allow to move a character in a world
	*/
	class PhysicsCharacterController
	{
		public:
			PhysicsCharacterController(const std::shared_ptr<PhysicsCharacter> &, PhysicsWorld *);
			~PhysicsCharacterController();

			void setMomentum(const Vector3<float> &);
			void jump();

			void update(float);

		private:
			void setup(float);

			Vector3<float> getVelocity() const;
			bool needJumpAndResetFlag();

			void recoverFromPenetration(float);
			SignificantContactValues resetSignificantContactValues();
			void saveSignificantContactValues(SignificantContactValues &, const Vector3<float> &);
			void computeSignificantContactValues(SignificantContactValues &, float);

			float computeSlope();

			const float recoverFactors[RECOVER_PENETRATION_SUB_STEPS] = {0.4, 0.7, 0.9, 1.0};
			const float timeKeepMoveInAir;
			const float percentageControlInAir;

            std::shared_ptr<PhysicsCharacter> physicsCharacter;
			PhysicsWorld *physicsWorld;
			std::vector<ManifoldResult> manifoldResults;
			mutable std::mutex characterMutex;
            WorkGhostBody *ghostBody;
			float verticalSpeed;

			Vector3<float> velocity;
			std::atomic_bool makeJump;

			Point3<float> previousBodyPosition;
			Quaternion<float> initialOrientation;
			Vector3<float> lastVelocity;
			unsigned int numberOfHit; //number of contact point touching the character
			bool isOnGround; //character is on the ground
			bool hitRoof; //character has hit the roof
			float timeInTheAir; //time (sec.) character is not on the ground
			bool jumping; //character is jumping
			float slopeInPercentage; //slope in percentage (a positive value means that character climb)
	};

}

#endif
