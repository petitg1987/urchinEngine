#include <vector>
#include <limits>
#include <cmath>

#include <character/CharacterController.h>
#include <shape/CollisionCapsuleShape.h>
#include <collision/ManifoldContactPoint.h>
#include <PhysicsWorld.h>

namespace urchin {

    //static
    const float CharacterController::MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND = 0.2f;
    const float CharacterController::MIN_WALK_SPEED_PERCENTAGE = 0.75f;
    const float CharacterController::MAX_WALK_SPEED_PERCENTAGE = 1.25f;
    const std::array<float, 4> CharacterController::RECOVER_FACTOR = {0.4f, 0.7f, 0.9f, 1.0f};

    CharacterController::CharacterController(std::shared_ptr<PhysicsCharacter> physicsCharacter, CharacterControllerConfig config, PhysicsWorld* physicsWorld) :
            maxDepthToRecover(ConfigService::instance()->getFloatValue("character.maxDepthToRecover")),
            physicsCharacter(std::move(physicsCharacter)),
            config(config),
            physicsWorld(physicsWorld),
            ghostBody(nullptr),
            verticalSpeed(0.0f),
            makeJump(false),
            initialOrientation(this->physicsCharacter->getTransform().getOrientation()),
            numberOfHit(0),
            isOnGround(false),
            hitRoof(false),
            timeInTheAir(0.0f),
            jumping(false),
            slopeInPercentage(0.0f) {
        if (!physicsWorld) {
            throw std::runtime_error("Physics world cannot be null for character controller.");
        }

        auto ghostBodyCapsuleShape = std::dynamic_pointer_cast<const CollisionCapsuleShape>(this->physicsCharacter->getShape());
        if (ghostBodyCapsuleShape) {
            float radius = ghostBodyCapsuleShape->getRadius(); //TODO adapt to max speed
            float height = ghostBodyCapsuleShape->getCylinderHeight() + (2.0f * radius);
            auto resizedCharacterShape = std::make_shared<const CollisionCapsuleShape>(radius, height - (2.0f * radius), ghostBodyCapsuleShape->getCapsuleOrientation());
            ghostBody = new GhostBody(this->physicsCharacter->getName(), this->physicsCharacter->getTransform(), resizedCharacterShape);
        } else {
            throw std::runtime_error("Unimplemented shape type for character controller: " + std::to_string(this->physicsCharacter->getShape()->getShapeType()));
        }

        ghostBody->setIsActive(true); //always active for get better reactivity
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->addBodyAsync(ghostBody);
    }

    CharacterController::~CharacterController() {
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->removeBodyAsync(ghostBody);
    }

    void CharacterController::setMomentum(const Vector3<float>& momentum) {
        std::lock_guard<std::mutex> lock(characterMutex);
        this->velocity = (momentum / physicsCharacter->getMass());
    }

    Vector3<float> CharacterController::getVelocity() const {
        std::lock_guard<std::mutex> lock(characterMutex);
        return velocity;
    }

    void CharacterController::jump() {
        makeJump.store(true, std::memory_order_relaxed);
    }

    bool CharacterController::needJumpAndResetFlag() {
        return makeJump.exchange(false, std::memory_order_relaxed);
    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void CharacterController::update(float dt) {
        ScopeProfiler sp(Profiler::physics(), "charactCtrlExec");

        if (!ghostBody->isStatic()) {
            //setup values
            setup(dt);

            //recover from penetration
            recoverFromPenetration(dt);

            //compute values
            slopeInPercentage = 0.0f;
            if (isOnGround) {
                verticalSpeed = 0.0f;
                slopeInPercentage = computeSlope();
            }
            if (hitRoof) {
                verticalSpeed = 0.0f;
            }

            //set new transform on character
            physicsCharacter->updateTransform(ghostBody->getTransform());
        }
    }

    void CharacterController::setup(float dt) {
        //save values
        previousBodyTransform = ghostBody->getTransform();

        //apply user move
        Point3<float> targetPosition = previousBodyTransform.getPosition();
        Vector3<float> velocity = getVelocity(); //TODO clamp velocity based on maximum horizontal speed
        if (isOnGround) {
            float slopeSpeedDecrease = 1.0f - (slopeInPercentage / config.getMaxSlopeInPercentage());
            slopeSpeedDecrease = MathFunction::clamp(slopeSpeedDecrease, MIN_WALK_SPEED_PERCENTAGE, MAX_WALK_SPEED_PERCENTAGE);
            targetPosition = targetPosition.translate(velocity * dt * slopeSpeedDecrease);

            lastVelocity = velocity;
        } else if (timeInTheAir < config.getTimeKeepMoveInAir()) {
            float momentumSpeedDecrease = 1.0f - (timeInTheAir / config.getTimeKeepMoveInAir());
            Vector3<float> walkDirectionInAir = velocity * (1.0f - config.getPercentageControlInAir()) + velocity * config.getPercentageControlInAir();
            targetPosition = targetPosition.translate(walkDirectionInAir * dt * momentumSpeedDecrease);
        } else {
            lastVelocity.setNull();
        }

        //jump
        bool closeToTheGround = timeInTheAir < MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND;
        if (needJumpAndResetFlag() && closeToTheGround && !jumping) {
            verticalSpeed += config.getJumpSpeed();
            isOnGround = false;
            jumping = true;
        } else if (isOnGround && jumping) {
            jumping = false;
        }

        //compute gravity velocity
        if (!isOnGround || numberOfHit > 1) {
            verticalSpeed -= (-physicsWorld->getGravity().Y) * dt;
            if (verticalSpeed < -config.getMaxVerticalSpeed()) {
                verticalSpeed = -config.getMaxVerticalSpeed();
            }
        }

        //compute and apply orientation
        Quaternion<float> newOrientation = initialOrientation;
        if (!MathFunction::isZero(velocity.squareLength(), 0.001f)) {
            Quaternion<float> orientation = Quaternion<float>(velocity.normalize()).normalize();
            newOrientation = orientation * initialOrientation;
        }

        //apply data on body
        targetPosition.Y += verticalSpeed * dt;
        ghostBody->setTransform(PhysicsTransform(targetPosition, newOrientation));
    }

    void CharacterController::recoverFromPenetration(float dt) {
        resetSignificantContactValues();

        PhysicsTransform characterTransform = ghostBody->getTransform();
        for (unsigned int subStepIndex = 0; subStepIndex < RECOVER_FACTOR.size(); ++subStepIndex) {
            manifoldResults.clear();
            physicsWorld->getCollisionWorld()->getNarrowPhaseManager()->processGhostBody(ghostBody, manifoldResults);

            for (const auto& manifoldResult : manifoldResults) {
                float sign = manifoldResult.getBody1() == ghostBody ? -1.0f : 1.0f;
                for (unsigned int i = 0; i < manifoldResult.getNumContactPoints(); ++i) {
                    const ManifoldContactPoint& manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
                    float depth = manifoldContactPoint.getDepth();

                    if (depth < maxDepthToRecover) {
                        Vector3<float> normal =  manifoldContactPoint.getNormalFromObject2() * sign;
                        Vector3<float> moveVector = normal * depth * RECOVER_FACTOR[subStepIndex];

                        characterTransform.setPosition(characterTransform.getPosition().translate(moveVector));
                        ghostBody->setTransform(characterTransform);

                        if (subStepIndex == 0) {
                            saveSignificantContactValues(normal);
                        }
                    }
                }
            }
        }

        computeSignificantContactValues(dt);
    }

    void CharacterController::resetSignificantContactValues() {
        significantContactValues.numberOfHit = 0;

        significantContactValues.maxDotProductUpNormalAxis = std::numeric_limits<float>::min();
        significantContactValues.mostUpVerticalNormal = Vector3<float>();

        significantContactValues.maxDotProductDownNormalAxis = -std::numeric_limits<float>::max();
        significantContactValues.mostDownVerticalNormal = Vector3<float>();
    }

    void CharacterController::saveSignificantContactValues(const Vector3<float>& normal) {
        significantContactValues.numberOfHit++;

        float dotProductUpNormalAxis = (-normal).dotProduct(Vector3<float>(0.0f, 1.0f, 0.0f));
        if (dotProductUpNormalAxis > significantContactValues.maxDotProductUpNormalAxis) {
            significantContactValues.maxDotProductUpNormalAxis = dotProductUpNormalAxis;
            significantContactValues.mostUpVerticalNormal = -normal;
        }

        float dotProductDownNormalAxis = (-normal).dotProduct(Vector3<float>(0.0f, -1.0f, 0.0f));
        if (dotProductDownNormalAxis > significantContactValues.maxDotProductDownNormalAxis) {
            significantContactValues.maxDotProductDownNormalAxis = dotProductDownNormalAxis;
            significantContactValues.mostDownVerticalNormal = -normal;
        }
    }

    void CharacterController::computeSignificantContactValues(float dt) {
        numberOfHit = significantContactValues.numberOfHit;
        isOnGround = numberOfHit > 0 && std::acos(significantContactValues.maxDotProductUpNormalAxis) < config.getMaxSlopeInRadian();
        hitRoof = numberOfHit > 0 && std::acos(significantContactValues.maxDotProductDownNormalAxis) < config.getMaxSlopeInRadian();
        timeInTheAir = isOnGround ? 0.0f : timeInTheAir + dt;
    }

    /**
     * Compute slope based on previous body position.
     * Slope is expressed in percentage. A positive value means that character climb.
     */
    float CharacterController::computeSlope() {
        Point3<float> bodyPosition = ghostBody->getTransform().getPosition();
        Point2<float> p1 = Point2<float>(bodyPosition.X, bodyPosition.Z);
        Point2<float> p2 = Point2<float>(previousBodyTransform.getPosition().X, previousBodyTransform.getPosition().Z);
        float run = p1.vector(p2).length();
        if (run == 0.0f) {
            return 0.0f;
        }

        float rise = bodyPosition.Y - previousBodyTransform.getPosition().Y;
        return rise / run;
    }
}
