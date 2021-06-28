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
    const float CharacterController::SAVE_RESPAWN_TRANSFORM_TIME = 10.0f; //10 seconds
    const std::array<float, 4> CharacterController::RECOVER_FACTOR = {0.4f, 0.7f, 0.9f, 1.0f};

    CharacterController::CharacterController(std::shared_ptr<PhysicsCharacter> physicsCharacter, CharacterControllerConfig config, PhysicsWorld* physicsWorld) :
            ccdMotionThresholdFactor(ConfigService::instance()->getFloatValue("collisionShape.ccdMotionThresholdFactor")) ,
            maxDepthToRecover(ConfigService::instance()->getFloatValue("character.maxDepthToRecover")),
            minUpdateFrequency(ConfigService::instance()->getFloatValue("character.minUpdateFrequency")),
            physicsCharacter(std::move(physicsCharacter)),
            config(config),
            physicsWorld(physicsWorld),
            ghostBody(nullptr),
            ccdGhostBody(nullptr),
            verticalSpeed(0.0f),
            makeJump(false),
            significantContactValues({}),
            respawnValues({}),
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

        createBodies();
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->addBodyAsync(ghostBody);
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->addBodyAsync(ccdGhostBody);
        respawnValues.nextRespawnTransform = ghostBody->getTransform();
    }

    CharacterController::~CharacterController() {
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->removeBodyAsync(ccdGhostBody);
        physicsWorld->getCollisionWorld()->getBroadPhaseManager()->removeBodyAsync(ghostBody);
    }

    void CharacterController::setVelocity(const Vector3<float>& velocity) {
        this->velocity = velocity;

        //clamp velocity to max horizontal speed
        Vector2<float> horizontalVelocity = velocity.xz();
        float velocityLength = horizontalVelocity.length();
        if(velocityLength > config.getMaxHorizontalSpeed()) {
            horizontalVelocity = (horizontalVelocity / velocityLength) * config.getMaxHorizontalSpeed();
            this->velocity = Vector3(horizontalVelocity.X, velocity.Y, horizontalVelocity.Y);
        }
    }

    void CharacterController::jump() {
        makeJump = true;
    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void CharacterController::update(float dt) {
        ScopeProfiler sp(Profiler::physics(), "charactCtrlExec");

        if (ghostBody->isActive() && ccdGhostBody->isActive()) {
            //update body transform
            updateBodiesTransform(dt);

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
        } else {
            respawnBodies();
        }

        //set new transform on character
        physicsCharacter->updateTransform(ghostBody->getTransform());
    }

    void CharacterController::createBodies() {
        ghostBody = new GhostBody(this->physicsCharacter->getName(), this->physicsCharacter->getTransform(), this->physicsCharacter->getShape());
        ghostBody->setIsActive(true); //always active for get better reactivity

        auto ghostBodyCapsule = std::dynamic_pointer_cast<const CollisionCapsuleShape>(this->physicsCharacter->getShape());
        std::shared_ptr<const CollisionShape3D> ccdGhostBodyShape;
        if (ghostBodyCapsule) {
            float radius = ghostBodyCapsule->getRadius();
            float height = ghostBodyCapsule->getCylinderHeight() + (2.0f * radius);
            float maximumCharacterSpeed = config.getMaxHorizontalSpeed() * (1.0f + config.getMaxSlopeSpeedVariation());

            float ccdRadius = std::max(radius, maximumCharacterSpeed / minUpdateFrequency);
            float ccdHeight = std::max(height, (config.getMaxVerticalSpeed() / minUpdateFrequency) * 2.0f);
            float ccdCylinderHeight = std::max(0.01f, ccdHeight - (2.0f * ccdRadius));
            ccdGhostBodyShape = std::make_shared<const CollisionCapsuleShape>(ccdRadius, ccdCylinderHeight, ghostBodyCapsule->getCapsuleOrientation());
        } else {
            throw std::runtime_error("Unimplemented shape type for character controller: " + std::to_string(this->physicsCharacter->getShape()->getShapeType()));
        }
        ccdGhostBody = new GhostBody(this->physicsCharacter->getName() + "_ccd", this->physicsCharacter->getTransform(), ccdGhostBodyShape);
        ccdGhostBody->setIsActive(true);
    }

    void CharacterController::updateBodiesTransform(float dt) {
        //save values
        previousBodyPosition = ghostBody->getTransform().getPosition();

        //user velocity
        Point3<float> targetPosition = previousBodyPosition;
        if (isOnGround) {
            float slopeSpeedVariation = 1.0f - (slopeInPercentage / config.getMaxSlopeInPercentage());
            slopeSpeedVariation = MathFunction::clamp(slopeSpeedVariation, 1.0f - config.getMaxSlopeSpeedVariation(), 1.0f + config.getMaxSlopeSpeedVariation());
            targetPosition = targetPosition.translate(velocity * dt * slopeSpeedVariation);
        } else if (timeInTheAir < config.getTimeKeepMoveInAir()) {
            float momentumSpeedDecrease = 1.0f - (timeInTheAir / config.getTimeKeepMoveInAir());
            Vector3<float> walkDirectionInAir = velocity * (1.0f - config.getPercentageControlInAir()) + velocity * config.getPercentageControlInAir();
            targetPosition = targetPosition.translate(walkDirectionInAir * dt * momentumSpeedDecrease);
        }

        //jump
        if(makeJump) {
            makeJump = false;
            bool closeToTheGround = timeInTheAir < MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND;
            if (closeToTheGround && !jumping) {
                verticalSpeed += config.getJumpSpeed();
                isOnGround = false;
                jumping = true;
            }
        } else if (isOnGround && jumping) {
            jumping = false;
        }

        //gravity velocity
        if (!isOnGround || numberOfHit > 1) {
            verticalSpeed -= (-physicsWorld->getGravity().Y) * dt;
            if (verticalSpeed < -config.getMaxVerticalSpeed()) {
                verticalSpeed = -config.getMaxVerticalSpeed();
            }
        }
        targetPosition.Y += verticalSpeed * dt;

        //CCD
        Vector3<float> moveVector = previousBodyPosition.vector(targetPosition);
        bool ccdRequired = moveVector.length() > ghostBody->getCcdMotionThreshold();
        if (ccdRequired) {
            manifoldResults.clear();
            physicsWorld->getCollisionWorld()->getNarrowPhaseManager()->processGhostBody(ccdGhostBody, manifoldResults);

            for (std::size_t i = 0; i < manifoldResults.size() && ccdRequired; ++i) {
                const ManifoldResult& manifoldResult = manifoldResults[i];
                for (unsigned int pointIndex = 0; pointIndex < manifoldResult.getNumContactPoints(); ++pointIndex) {
                    const ManifoldContactPoint& contactPoint = manifoldResult.getManifoldContactPoint(pointIndex);

                    Vector3<float> contactNormal = (manifoldResult.getBody1() == ccdGhostBody) ? -contactPoint.getNormalFromObject2() : contactPoint.getNormalFromObject2();
                    Point3<float> obstacleContactPoint = (manifoldResult.getBody1() == ccdGhostBody) ? contactPoint.getPointOnObject2() : contactPoint.getPointOnObject1();
                    float motionAppliedOnObstacle = moveVector.dotProduct(contactNormal);
                    float motionToPassThroughObstacle = previousBodyPosition.vector(obstacleContactPoint).length();

                    if (motionAppliedOnObstacle > motionToPassThroughObstacle * ccdMotionThresholdFactor) {
                        Vector3<float> obstacleReactionMotion = (-contactNormal) * motionAppliedOnObstacle;
                        moveVector += obstacleReactionMotion;
                        targetPosition = previousBodyPosition.translate(moveVector);

                        ccdRequired = moveVector.length() > ghostBody->getCcdMotionThreshold();
                    }
                }
            }
        }

        //orientation
        Quaternion<float> newOrientation = initialOrientation;
        if (!MathFunction::isZero(velocity.squareLength(), 0.001f)) {
            Quaternion<float> orientation = Quaternion<float>(velocity.normalize()).normalize();
            newOrientation = orientation * initialOrientation;
        }

        //save respawn transform
        respawnValues.timeElapse += dt;
        if (respawnValues.timeElapse > SAVE_RESPAWN_TRANSFORM_TIME) {
            respawnValues.respawnTransform = respawnValues.nextRespawnTransform;
            respawnValues.nextRespawnTransform = PhysicsTransform(targetPosition, newOrientation);
            respawnValues.timeElapse = 0.0f;
        }

        //apply transform on bodies
        ghostBody->setTransform(PhysicsTransform(targetPosition, newOrientation));
        ccdGhostBody->setTransform(PhysicsTransform(targetPosition, newOrientation));
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
                        ccdGhostBody->setTransform(characterTransform);

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
        Point2<float> p1 = bodyPosition.toPoint2XZ();
        Point2<float> p2 = previousBodyPosition.toPoint2XZ();
        float run = p1.vector(p2).length();
        if (run == 0.0f) {
            return 0.0f;
        }

        float rise = bodyPosition.Y - previousBodyPosition.Y;
        return rise / run;
    }

    void CharacterController::respawnBodies() {
        respawnValues.timeElapse = 0.0f;
        respawnValues.nextRespawnTransform = respawnValues.respawnTransform;

        ghostBody->setTransform(respawnValues.respawnTransform);
        ghostBody->setIsActive(true);

        ccdGhostBody->setTransform(respawnValues.respawnTransform);
        ccdGhostBody->setIsActive(true);
    }
}
