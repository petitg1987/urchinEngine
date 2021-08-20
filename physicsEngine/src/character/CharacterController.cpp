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

    CharacterController::CharacterController(std::shared_ptr<PhysicsCharacter> physicsCharacter, CharacterControllerConfig config, PhysicsWorld& physicsWorld) :
            ccdMotionThresholdFactor(ConfigService::instance().getFloatValue("collisionShape.ccdMotionThresholdFactor")) ,
            maxDepthToRecover(ConfigService::instance().getFloatValue("character.maxDepthToRecover")),
            minUpdateFrequency(ConfigService::instance().getFloatValue("character.minUpdateFrequency")),
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
        createBodies();
        physicsWorld.getCollisionWorld().getBroadPhase().addBodyAsync(ghostBody);
        physicsWorld.getCollisionWorld().getBroadPhase().addBodyAsync(ccdGhostBody);

        respawnValues.respawnTransform = ghostBody->getTransform();
        respawnValues.nextRespawnTransform = respawnValues.respawnTransform;
    }

    CharacterController::~CharacterController() {
        physicsWorld.getCollisionWorld().getBroadPhase().removeBodyAsync(ccdGhostBody);
        physicsWorld.getCollisionWorld().getBroadPhase().removeBodyAsync(ghostBody);
    }

    void CharacterController::setVelocity(const Vector3<float>& velocity) {
        this->velocity = velocity;

        //clamp velocity to max horizontal speed
        Vector2<float> horizontalVelocity = velocity.xz();
        float velocityLength = horizontalVelocity.length();
        if (velocityLength > config.getMaxHorizontalSpeed()) {
            horizontalVelocity = (horizontalVelocity / velocityLength) * config.getMaxHorizontalSpeed();
            this->velocity = Vector3(horizontalVelocity.X, velocity.Y, horizontalVelocity.Y);
        }
    }

    /**
     * Define the character orientation. This orientation has only effect when the character is not moving.
     * When the character is moving, the orientation is determined in function of velocity direction.
     */
    void CharacterController::setOrientation(const Vector3<float>& viewVector) {
        //Following lines are equivalent to Camera#updateComponents() but with simplification to avoid the rotation on X/Z axis:
        // - position.Y / viewVector.Y set to 0.0
        // - up = 0.0, 1.0, 0.0
        Vector3<float> yViewVector = Vector3<float>(viewVector.X, 0.0f, viewVector.Z).normalize();
        Matrix3<float> mView(-yViewVector.Z,    0.0f,       yViewVector.X,
                             0.0f,              1.0f,       0.0f,
                             -yViewVector[0],   0.0f,       -yViewVector[2]);
        Quaternion<float> yRotation = Quaternion<float>(mView).conjugate();

        Point3<float> position = ghostBody->getTransform().getPosition();
        ghostBody->setTransform(PhysicsTransform(position, yRotation));
        ccdGhostBody->setTransform(PhysicsTransform(position, yRotation));
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
        ghostBody = std::make_shared<GhostBody>(physicsCharacter->getName(), physicsCharacter->getTransform(), physicsCharacter->getShape().clone());
        ghostBody->setIsActive(true); //always active for get better reactivity

        std::unique_ptr<const CollisionShape3D> ccdGhostBodyShape;
        try {
            auto& ghostBodyCapsule = dynamic_cast<const CollisionCapsuleShape&>(ghostBody->getShape());

            float radius = ghostBodyCapsule.getRadius();
            float height = ghostBodyCapsule.getCylinderHeight() + (2.0f * radius);
            float maximumCharacterSpeed = config.getMaxHorizontalSpeed() * (1.0f + config.getMaxSlopeSpeedVariation());

            float ccdRadius = std::max(radius, maximumCharacterSpeed / minUpdateFrequency);
            float ccdHeight = std::max(height, (config.getMaxVerticalSpeed() / minUpdateFrequency) * 2.0f);
            float ccdCylinderHeight = std::max(0.01f, ccdHeight - (2.0f * ccdRadius));
            ccdGhostBodyShape = std::make_unique<const CollisionCapsuleShape>(ccdRadius, ccdCylinderHeight, ghostBodyCapsule.getCapsuleOrientation());
        } catch (const std::bad_cast& e) {
            throw std::runtime_error("Unimplemented shape type for character controller: " + std::to_string(ghostBody->getShape().getShapeType()));
        }
        ccdGhostBody = std::make_shared<GhostBody>(this->physicsCharacter->getName() + "_ccd", this->physicsCharacter->getTransform(), std::move(ccdGhostBodyShape));
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
        if (makeJump) {
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
            verticalSpeed -= (-physicsWorld.getGravity().Y) * dt;
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
            physicsWorld.getCollisionWorld().getNarrowPhase().processGhostBody(*ccdGhostBody, manifoldResults);

            for (std::size_t i = 0; i < manifoldResults.size() && ccdRequired; ++i) {
                const ManifoldResult& manifoldResult = manifoldResults[i];
                for (unsigned int pointIndex = 0; pointIndex < manifoldResult.getNumContactPoints(); ++pointIndex) {
                    const ManifoldContactPoint& contactPoint = manifoldResult.getManifoldContactPoint(pointIndex);

                    Vector3<float> contactNormal = (&manifoldResult.getBody1() == ccdGhostBody.get()) ? -contactPoint.getNormalFromObject2() : contactPoint.getNormalFromObject2();
                    Point3<float> obstacleContactPoint = (&manifoldResult.getBody1() == ccdGhostBody.get()) ? contactPoint.getPointOnObject2() : contactPoint.getPointOnObject1();
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
        Quaternion<float> updatedOrientation = ghostBody->getTransform().getOrientation();
        if (!MathFunction::isZero(velocity.squareLength(), 0.001f)) {
            Quaternion<float> orientation = Quaternion<float>(velocity.normalize()).normalize();
            updatedOrientation = orientation * initialOrientation;
        }

        //save respawn transform
        respawnValues.timeElapse += dt;
        if (respawnValues.timeElapse > SAVE_RESPAWN_TRANSFORM_TIME) {
            respawnValues.respawnTransform = respawnValues.nextRespawnTransform;
            respawnValues.nextRespawnTransform = PhysicsTransform(targetPosition, updatedOrientation);
            respawnValues.timeElapse = 0.0f;
        }

        //apply transform on bodies
        ghostBody->setTransform(PhysicsTransform(targetPosition, updatedOrientation));
        ccdGhostBody->setTransform(PhysicsTransform(targetPosition, updatedOrientation));
    }

    void CharacterController::recoverFromPenetration(float dt) {
        resetSignificantContactValues();

        PhysicsTransform characterTransform = ghostBody->getTransform();
        for (unsigned int subStepIndex = 0; subStepIndex < RECOVER_FACTOR.size(); ++subStepIndex) {
            manifoldResults.clear();
            physicsWorld.getCollisionWorld().getNarrowPhase().processGhostBody(*ghostBody, manifoldResults);

            for (const auto& manifoldResult : manifoldResults) {
                float sign = &manifoldResult.getBody1() == ghostBody.get() ? -1.0f : 1.0f;
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
