#pragma once

#include <memory>

#include <character/PhysicsCharacter.h>
#include <character/CharacterControllerConfig.h>
#include <body/model/GhostBody.h>
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
        float timeElapse{};
    };

    /**
    * Character controller: allow to move a character in a world
    */
    class CharacterController {
        public:
            CharacterController(std::unique_ptr<PhysicsCharacter>, CharacterControllerConfig, PhysicsWorld&);
            ~CharacterController();

            void walk(const Vector3<float>&);
            void run(const Vector3<float>&);
            void verticalMove(float);
            void jump();
            void enableGravity(bool);
            void updateTransform(const Point3<float>&, const Vector3<float>&) const;
            void updateOrientation(const Vector3<float>&) const;

            PhysicsCharacter& getPhysicsCharacter() const;
            bool isOnGround() const;
            bool isGravityEnabled() const;

            void update(float);

        private:
            void createBodies();

            Quaternion<float> computeYRotation(const Vector3<float>&) const;

            void updateStep(float);
            void updateBodiesTransform(float);
            void recoverFromPenetration(float);
            void resetSignificantContactValues();
            void saveSignificantContactValues(const Vector3<float>&);
            void computeSignificantContactValues(float);
            float computeSlope() const;
            void respawnBodies();

            static constexpr float MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND = 0.2f;
            static constexpr float SAVE_RESPAWN_TRANSFORM_TIME = 10.0f; // 10 seconds
            static constexpr std::array<float, 4> RECOVER_FACTORS = {0.4f, 0.7f, 0.9f, 1.0f};
            static constexpr std::size_t MAX_UPDATE_LOOP_BY_FRAME = 50;
            static constexpr unsigned int MAX_ERRORS_LOG = 10;
            static constexpr float MAX_DEPTH_TO_RECOVER = 0.002f;
            static constexpr float MIN_UPDATE_FREQUENCY = 30;

            std::unique_ptr<PhysicsCharacter> physicsCharacter;
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
            bool onGround; //character is on the ground
            bool hitRoof; //character has hit the roof
            float timeInTheAir; //time (sec.) character is not on the ground
            bool jumping; //character is jumping
            bool gravityEnabled; //without gravity, character is able to navigate freely up and down
            float slopeInPercentage; //slope in percentage (a positive value means that character climb)
    };

}
