#ifndef URCHINENGINE_CHARATERCONTROLLER_H
#define URCHINENGINE_CHARATERCONTROLLER_H

#include <memory>
#include <mutex>

#include "processable/Processable.h"
#include "utils/math/PhysicsTransform.h"
#include "shape/CollisionShape3D.h"
#include "body/work/WorkGhostBody.h"

#define RECOVER_PENETRATION_SUB_STEPS 4 //number of steps to recover character from penetration

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
	class CharacterController : public Processable
	{
		public:
			CharacterController(const std::shared_ptr<const CollisionShape3D> &, const PhysicsTransform &);
			~CharacterController();

			void initialize(PhysicsWorld *);

			void setWalkDirection(const Vector3<float> &);
			const Vector3<float> &getWalkDirection() const;

			void setJumpSpeed(float);
			float getJumpSpeed() const;
			void jump();

			void setMaxSlope(float);
			float getMaxSlopeInRadian() const;
			float getMaxSlopeInPercentage() const;

			const PhysicsTransform &getTransform() const;

			void setup(float, const Vector3<float> &) override;
			void execute(float, const Vector3<float> &) override;

		private:
			bool needJumpAndUpdateFlag();
			void setTransform(const PhysicsTransform &);

			void recoverFromPenetration(float);
			SignificantContactValues resetSignificantContactValues();
			void saveSignificantContactValues(SignificantContactValues &, const Vector3<float> &);
			void computeSignificantContactValues(SignificantContactValues &, float);

			float computeSlope();

			const float recoverFactors[RECOVER_PENETRATION_SUB_STEPS] = {0.4, 0.7, 0.9, 1.0};
			const float timeKeepMoveInAir;
			const float percentageControlInAir;

			PhysicsWorld *physicsWorld;
			std::vector<ManifoldResult> manifoldResults;
			mutable std::mutex characterMutex;
			WorkGhostBody *ghostBody;
			PhysicsTransform ghostBodyTransform;
			float verticalVelocity;

			Vector3<float> walkDirection;
			float maxSlopeInRadian, maxSlopeInPercentage;
			float jumpSpeed;
			bool makeJump;

			Point3<float> previousBodyPosition;
			Vector3<float> lastWalkDirection;
			unsigned int numberOfHit; //number of contact point touching the character
			bool isOnGround; //character is on the ground
			bool hitRoof; //character has hit the roof
			float timeInTheAir; //time (sec.) character is not on the ground
			float jumping; //character is jumping
			float slopeInPercentage; //slope in percentage (a positive value means that character climb)
	};

}

#endif
