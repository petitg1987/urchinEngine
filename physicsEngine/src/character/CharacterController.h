#pragma once

#include <memory>
#include <mutex>
#include <atomic>

#include <character/PhysicsCharacter.h>
#include <character/CharacterControllerConfig.h>
#include <body/model/GhostBody.h>
#include <processable/Processable.h>
#include <collision/ManifoldResult.h>
#include <collision/OverlappingPair.h>

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
            CharacterController(std::shared_ptr<PhysicsCharacter>, const CharacterControllerConfig&, PhysicsWorld&);
            ~CharacterController();

            void setVelocity(const Vector3<float>&);
            void setOrientation(const Vector3<float>&);
            void jump();
            void enableGravity(bool);

            bool isOnGround() const;
            bool isGravityEnabled() const;

            void update(float);

        private:
            void createBodies();

            void updateStep(float);
            void updateBodiesTransform(float);
            void recoverFromPenetration(float);
            void resetSignificantContactValues();
            void saveSignificantContactValues(const Vector3<float>&);
            void computeSignificantContactValues(float);
            float computeSlope();
            void respawnBodies();

            static constexpr float MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND = 0.2f;
            static constexpr float SAVE_RESPAWN_TRANSFORM_TIME = 10.0f; // 10 seconds
            static constexpr std::array<float, 4> RECOVER_FACTOR = {0.4f, 0.7f, 0.9f, 1.0f};
            static constexpr std::size_t MAX_UPDATE_LOOP_BY_FRAME = 25;
            static constexpr unsigned int MAX_ERRORS_LOG = 10;
            const float ccdMotionThresholdFactor;
            const float maxDepthToRecover;
            const float minUpdateFrequency;

            std::shared_ptr<PhysicsCharacter> physicsCharacter;
            CharacterControllerConfig config;
            PhysicsWorld& physicsWorld;
            std::vector<ManifoldResult> manifoldResults;
            std::shared_ptr<GhostBody> ghostBody;
            std::shared_ptr<GhostBody> ccdGhostBody;
            float verticalSpeed;

            Vector3<float> velocity;
            bool makeJump;

            SignificantContactValues significantContactValues;
            RespawnValues respawnValues;
            Point3<float> previousBodyPosition;
            Quaternion<float> initialOrientation;
            unsigned int numberOfHit; //number of contact point touching the character
            bool bIsOnGround; //character is on the ground
            bool hitRoof; //character has hit the roof
            float timeInTheAir; //time (sec.) character is not on the ground
            bool jumping; //character is jumping
            bool gravityEnabled; //without gravity, character is able to navigate freely up and down
            float slopeInPercentage; //slope in percentage (a positive value means that character climb)
    };

}
