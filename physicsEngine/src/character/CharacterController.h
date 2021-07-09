#pragma once

#include <memory>
#include <mutex>
#include <atomic>

#include <character/PhysicsCharacter.h>
#include <character/CharacterControllerConfig.h>
#include <body/model/GhostBody.h>
#include <processable/Processable.h>
#include <collision/ManifoldResult.h>

namespace urchin {

    class PhysicsWorld;

    struct SignificantContactValues {
        unsigned int numberOfHit;

        float maxDotProductUpNormalAxis;
        Vector3<float> mostUpVerticalNormal;

        float maxDotProductDownNormalAxis;
        Vector3<float> mostDownVerticalNormal;
    };

    struct RespawnValues {
        PhysicsTransform respawnTransform;
        PhysicsTransform nextRespawnTransform;
        float timeElapse;
    };

    /**
    * Character controller: allow to move a character in a world
    */
    class CharacterController {
        public:
            CharacterController(std::shared_ptr<PhysicsCharacter>, CharacterControllerConfig, PhysicsWorld*);
            ~CharacterController();

            void setVelocity(const Vector3<float>&);
            void jump();

            void update(float);

        private:
            void createBodies();

            void updateBodiesTransform(float);
            void recoverFromPenetration(float);
            void resetSignificantContactValues();
            void saveSignificantContactValues(const Vector3<float>&);
            void computeSignificantContactValues(float);
            float computeSlope();
            void respawnBodies();

            static const float MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND;
            static const float SAVE_RESPAWN_TRANSFORM_TIME;
            static const std::array<float, 4> RECOVER_FACTOR;
            const float ccdMotionThresholdFactor;
            const float maxDepthToRecover;
            const float minUpdateFrequency;

            std::shared_ptr<PhysicsCharacter> physicsCharacter;
            CharacterControllerConfig config;
            PhysicsWorld* physicsWorld;
            std::vector<ManifoldResult> manifoldResults;
            GhostBody* ghostBody;
            GhostBody* ccdGhostBody;
            float verticalSpeed;

            Vector3<float> velocity;
            bool makeJump;

            SignificantContactValues significantContactValues;
            RespawnValues respawnValues;
            Point3<float> previousBodyPosition;
            Quaternion<float> initialOrientation;
            unsigned int numberOfHit; //number of contact point touching the character
            bool isOnGround; //character is on the ground
            bool hitRoof; //character has hit the roof
            float timeInTheAir; //time (sec.) character is not on the ground
            bool jumping; //character is jumping
            float slopeInPercentage; //slope in percentage (a positive value means that character climb)
    };

}
