#include <vector>
#include <limits>
#include <cmath>

#include "PhysicsWorld.h"
#include "character/CharacterController.h"
#include "shape/CollisionCapsuleShape.h"
#include "collision/ManifoldContactPoint.h"

namespace urchin {

    CharacterController::CharacterController(std::unique_ptr<PhysicsCharacter> physicsCharacter, CharacterControllerConfig config, PhysicsWorld& physicsWorld) :
            physicsCharacter(std::move(physicsCharacter)),
            config(std::move(config)),
            physicsWorld(physicsWorld),
            ghostBody(nullptr),
            ccdGhostBody(nullptr),
            standingGhostBody(nullptr),
            standingCcdGhostBody(nullptr),
            crouchedGhostBody(nullptr),
            crouchedCcdGhostBody(nullptr),
            standingHeight(0.0f),
            crouchedHeight(0.0f),
            verticalSpeed(0.0f),
            makeJump(false),
            significantContactValues({}),
            respawnValues({}),
            initialOrientation(this->physicsCharacter->getTransform().getOrientation()),
            numberOfHit(0),
            onGround(false),
            hitRoof(false),
            timeInTheAir(0.0f),
            jumping(false),
            crouchRequested(false),
            crouched(false),
            gravityEnabled(true),
            slopeInPercentage(0.0f) {
        createBodies();
        physicsWorld.getCollisionWorld().getBroadPhase().addBodyAsync(standingGhostBody);
        physicsWorld.getCollisionWorld().getBroadPhase().addBodyAsync(standingCcdGhostBody);
        if (crouchedGhostBody) {
            physicsWorld.getCollisionWorld().getBroadPhase().addBodyAsync(crouchedGhostBody);
            physicsWorld.getCollisionWorld().getBroadPhase().addBodyAsync(crouchedCcdGhostBody);
        }

        respawnValues.respawnTransform = standingGhostBody->getTransform();
        respawnValues.nextRespawnTransform = respawnValues.respawnTransform;
    }

    CharacterController::~CharacterController() {
        if (crouchedGhostBody) {
            physicsWorld.getCollisionWorld().getBroadPhase().removeBodyAsync(crouchedCcdGhostBody);
            physicsWorld.getCollisionWorld().getBroadPhase().removeBodyAsync(crouchedGhostBody);
        }
        physicsWorld.getCollisionWorld().getBroadPhase().removeBodyAsync(standingCcdGhostBody);
        physicsWorld.getCollisionWorld().getBroadPhase().removeBodyAsync(standingGhostBody);
    }

    /**
     * @param velocity Walk velocity in units/second.
     */
    void CharacterController::walk(const Vector3<float>& velocity) {
        updateVelocity(velocity, crouched ? config.getMaxWalkSpeedWhenCrouched() : config.getMaxWalkSpeed());
    }

    /**
     * @param velocity Run velocity in units/second.
     */
    void CharacterController::run(const Vector3<float>& velocity) {
        updateVelocity(velocity, crouched ? config.getMaxWalkSpeedWhenCrouched() : config.getMaxRunSpeed());
    }

    void CharacterController::updateVelocity(const Vector3<float>& velocity, float maxSpeed) {
        Vector3<float> newVelocity = velocity;
        if (isGravityEnabled()) {
            newVelocity.Y = 0.0f;
        }
        if (newVelocity.squareLength() > maxSpeed * maxSpeed) {
            newVelocity = newVelocity.normalize() * maxSpeed;
        }
        this->velocity = newVelocity;
    }

    void CharacterController::verticalMove(float verticalSpeed) {
        if (!isGravityEnabled()) {
            bool descendAllowed = verticalSpeed < 0.0f && !onGround;
            bool ascendAllowed = verticalSpeed > 0.0f && !hitRoof;
            if (descendAllowed || ascendAllowed) {
                this->velocity.Y += verticalSpeed;
            }
        }
    }

    void CharacterController::jump() {
        if (gravityEnabled) {
            makeJump = true;
        }
    }

    void CharacterController::crouch(bool crouch) {
        if (physicsCharacter->isCrouchEnabled()) {
            crouchRequested = crouch;
        }
    }

    void CharacterController::crouchImmediately() {
        if (physicsCharacter->isCrouchEnabled()) {
            crouchRequested = true;
            if (!crouched) {
                switchBodies(true);
            }
        }
    }

    void CharacterController::enableGravity(bool gravityEnabled) {
        this->gravityEnabled = gravityEnabled;
        if (!gravityEnabled) {
            this->verticalSpeed = 0.0f;
            this->makeJump = false;
            this->jumping = false;
        }
    }

    void CharacterController::updateTransform(const Point3<float>& standingPosition, const Vector3<float>& viewVector) const {
        Quaternion<float> yRotation = computeYRotation(viewVector);
        applyTransform(toActiveBodyTransform(PhysicsTransform(standingPosition, yRotation)));
    }

    /**
     * Define the character orientation. This orientation has only effect when the character is not moving.
     * When the character is moving, the orientation is determined in function of the direction.
     */
    void CharacterController::updateOrientation(const Vector3<float>& viewVector) const {
        Point3<float> position = ghostBody->getTransform().getPosition();
        Quaternion<float> yRotation = computeYRotation(viewVector);
        applyTransform(PhysicsTransform(position, yRotation));
    }

    Quaternion<float> CharacterController::computeYRotation(const Vector3<float>& viewVector) const {
        //Following lines are equivalent to Camera#updateComponents() but with simplification to avoid the rotation on X/Z axis:
        // - position.Y / viewVector.Y set to 0.0
        // - up = 0.0, 1.0, 0.0
        Vector3<float> yViewVector = Vector3(viewVector.X, 0.0f, viewVector.Z).normalize();
        Matrix3 mView(-yViewVector.Z,    0.0f,       yViewVector.X,
                      0.0f,              1.0f,       0.0f,
                      -yViewVector[0],   0.0f,       -yViewVector[2]);
        return Quaternion<float>::fromRotationMatrix(mView).conjugate();
    }

    const CharacterControllerConfig& CharacterController::getConfig() const {
        return config;
    }

    PhysicsCharacter& CharacterController::getPhysicsCharacter() const {
        return *physicsCharacter;
    }

    bool CharacterController::isOnGround() const {
        return onGround;
    }

    const std::string& CharacterController::getGroundBodyId() const {
        return groundBodyId;
    }

    bool CharacterController::isGravityEnabled() const {
        return gravityEnabled;
    }

    bool CharacterController::isCrouched() const {
        return crouched;
    }

    float CharacterController::getCurrentCharacterHeight() const {
        return crouched ? crouchedHeight : standingHeight;
    }

    /**
     * @param dt Delta of time between two simulation steps
     */
    void CharacterController::update(float dt) {
        ScopeProfiler sp(Profiler::physics(), "charactCtrlUp");

        std::size_t stepLoopCounter = 0;
        float remainingDt = dt;
        do {
            if (stepLoopCounter >= MAX_UPDATE_LOOP_BY_FRAME) [[unlikely]] {
                static unsigned int numErrorsLogged = 0;
                if (numErrorsLogged++ < MAX_ERRORS_LOG) {
                    Logger::instance().logWarning("Maximum of iteration reached on character update (dt: " + std::to_string(dt) + ", discarded dt: " + std::to_string(remainingDt) + ")");
                }
                break;
            }
            float stepDt = std::min(remainingDt, 1.0f / MIN_UPDATE_FREQUENCY);
            remainingDt -= stepDt;

            updateStep(stepDt);

            stepLoopCounter++;
        } while (remainingDt > 0.001f);
    }

    void CharacterController::createBodies() {
        const CollisionShape3D& standingShape = physicsCharacter->getStandingShape();
        standingHeight = computeHeight(standingShape);
        standingGhostBody = std::make_shared<GhostBody>(physicsCharacter->getName(), physicsCharacter->getTransform(), standingShape.clone());
        standingGhostBody->setIsActive(true); //always active for get better reactivity
        standingCcdGhostBody = std::make_shared<GhostBody>(physicsCharacter->getName() + "_ccd", physicsCharacter->getTransform(), createCcdShape(standingShape));
        standingCcdGhostBody->setIsActive(true);

        if (physicsCharacter->isCrouchEnabled()) {
            //crouched capsule keeps the radius of the standing capsule (validated by PhysicsCharacter): only its height is reduced
            const auto& standingCapsule = static_cast<const CollisionCapsuleShape&>(standingShape);
            crouchedHeight = physicsCharacter->getCrouchHeight();
            auto crouchedShape = std::make_unique<const CollisionCapsuleShape>(standingCapsule.getRadius(), crouchedHeight - (2.0f * standingCapsule.getRadius()), standingCapsule.getCapsuleOrientation());

            PhysicsTransform crouchedTransform = physicsCharacter->getTransform();
            crouchedTransform.setPosition(crouchedTransform.getPosition().translate(Vector3(0.0f, (crouchedHeight - standingHeight) / 2.0f, 0.0f)));
            crouchedCcdGhostBody = std::make_shared<GhostBody>(physicsCharacter->getName() + "_crouch_ccd", crouchedTransform, createCcdShape(*crouchedShape));
            crouchedCcdGhostBody->setIsActive(true);
            crouchedGhostBody = std::make_shared<GhostBody>(physicsCharacter->getName() + "_crouch", crouchedTransform, std::move(crouchedShape));
            crouchedGhostBody->setIsActive(true);
        }

        ghostBody = standingGhostBody;
        ccdGhostBody = standingCcdGhostBody;
    }

    std::unique_ptr<const CollisionShape3D> CharacterController::createCcdShape(const CollisionShape3D& shape) const {
        if (shape.getShapeType() != CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            throw std::runtime_error("Unimplemented shape type for character controller: " + std::to_string(shape.getShapeType()));
        }
        const auto& capsule = static_cast<const CollisionCapsuleShape&>(shape);

        float radius = capsule.getRadius();
        float height = capsule.getCylinderHeight() + (2.0f * radius);
        float maximumCharacterSpeed = config.getMaxRunSpeed() * (1.0f + config.getMaxSlopeSpeedVariation());

        float ccdRadius = std::max(radius, maximumCharacterSpeed / MIN_UPDATE_FREQUENCY);
        float ccdHeight = std::max(height, (config.getMaxVerticalSpeed() / MIN_UPDATE_FREQUENCY) * 2.0f);
        float ccdCylinderHeight = std::max(0.01f, ccdHeight - (2.0f * ccdRadius));
        return std::make_unique<const CollisionCapsuleShape>(ccdRadius, ccdCylinderHeight, capsule.getCapsuleOrientation());
    }

    float CharacterController::computeHeight(const CollisionShape3D& shape) {
        if (shape.getShapeType() != CollisionShape3D::ShapeType::CAPSULE_SHAPE) {
            throw std::runtime_error("Unimplemented shape type for character controller: " + std::to_string(shape.getShapeType()));
        }
        const auto& capsule = static_cast<const CollisionCapsuleShape&>(shape);
        return capsule.getCylinderHeight() + (2.0f * capsule.getRadius());
    }

    void CharacterController::applyTransform(const PhysicsTransform& transform) const {
        ghostBody->setTransform(transform);
        ccdGhostBody->setTransform(transform);

        if (crouchedGhostBody) {
            float inactiveHeight = crouched ? standingHeight : crouchedHeight;
            PhysicsTransform inactiveTransform = transform;
            inactiveTransform.setPosition(transform.getPosition().translate(Vector3(0.0f, (inactiveHeight - getCurrentCharacterHeight()) / 2.0f, 0.0f)));
            if (crouched) {
                standingGhostBody->setTransform(inactiveTransform);
                standingCcdGhostBody->setTransform(inactiveTransform);
            } else {
                crouchedGhostBody->setTransform(inactiveTransform);
                crouchedCcdGhostBody->setTransform(inactiveTransform);
            }
        }
    }

    PhysicsTransform CharacterController::toActiveBodyTransform(const PhysicsTransform& standingTransform) const {
        PhysicsTransform activeTransform = standingTransform;
        activeTransform.setPosition(standingTransform.getPosition().translate(Vector3(0.0f, (getCurrentCharacterHeight() - standingHeight) / 2.0f, 0.0f)));
        return activeTransform;
    }

    PhysicsTransform CharacterController::toStandingBodyTransform(const PhysicsTransform& activeTransform) const {
        PhysicsTransform standingTransform = activeTransform;
        standingTransform.setPosition(activeTransform.getPosition().translate(Vector3(0.0f, (standingHeight - getCurrentCharacterHeight()) / 2.0f, 0.0f)));
        return standingTransform;
    }

    void CharacterController::updateStep(float stepDt) {
        if (ghostBody->isActive() && ccdGhostBody->isActive()) {
            //crouch / stand up
            updateCrouchState();

            //update body transform
            updateBodiesTransform(stepDt);

            //recover from penetration
            recoverFromPenetration(stepDt);

            //compute values
            slopeInPercentage = 0.0f;
            if (onGround) {
                verticalSpeed = std::max(0.0f, verticalSpeed);
                slopeInPercentage = computeSlope();
            }
            if (hitRoof) {
                verticalSpeed = std::min(0.0f, verticalSpeed);
            }
        } else {
            respawnBodies();
        }

        //set new transform on character
        physicsCharacter->updateTransform(standingGhostBody->getTransform());
    }

    void CharacterController::updateCrouchState() {
        if (!crouchedGhostBody) {
            return;
        }

        if (crouchRequested && !crouched) {
            switchBodies(true);
        } else if (!crouchRequested && crouched && canStandUp()) {
            switchBodies(false);
        }
    }

    void CharacterController::switchBodies(bool crouched) {
        this->crouched = crouched;
        ghostBody = crouched ? crouchedGhostBody : standingGhostBody;
        ccdGhostBody = crouched ? crouchedCcdGhostBody : standingCcdGhostBody;
    }

    bool CharacterController::canStandUp() {
        manifoldResults.clear();
        physicsWorld.getCollisionWorld().getNarrowPhase().processGhostBody(*standingGhostBody, manifoldResults);

        float groundMinDotProduct = std::cos(config.getMaxSlopeInRadian());
        for (const auto& manifoldResult : manifoldResults) {
            float sign = &manifoldResult.getBody1() == standingGhostBody.get() ? -1.0f : 1.0f;
            for (unsigned int i = 0; i < manifoldResult.getNumContactPoints(); ++i) {
                const ManifoldContactPoint& manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
                Vector3<float> normal = manifoldContactPoint.getNormalFromObject2() * sign;
                bool isGroundContact = -normal.Y >= groundMinDotProduct;
                if (!isGroundContact && manifoldContactPoint.getDepth() < -STAND_UP_PENETRATION_TOLERANCE) {
                    return false;
                }
            }
        }
        return true;
    }

    void CharacterController::updateBodiesTransform(float dt) {
        //values
        previousBodyPosition = ghostBody->getTransform().getPosition();
        bool closeToTheGround = timeInTheAir < MAX_TIME_IN_AIR_CONSIDERED_AS_ON_GROUND;

        //user velocity
        Point3<float> targetPosition = previousBodyPosition;
        if (closeToTheGround) {
            float slopeSpeedVariation = 1.0f - (slopeInPercentage / config.getMaxSlopeInPercentage());
            slopeSpeedVariation = std::clamp(slopeSpeedVariation, 1.0f - config.getMaxSlopeSpeedVariation(), 1.0f + config.getMaxSlopeSpeedVariation());
            targetPosition = targetPosition.translate(velocity * dt * slopeSpeedVariation);
        } else if (!gravityEnabled) {
            targetPosition = targetPosition.translate(velocity * dt);
        } else if (timeInTheAir < config.getTimeKeepMoveInAir()) {
            float momentumSpeedDecrease = 1.0f - (timeInTheAir / config.getTimeKeepMoveInAir());
            Vector3<float> walkDirectionInAir = velocity * (1.0f - config.getPercentageControlInAir()) + velocity * config.getPercentageControlInAir();
            targetPosition = targetPosition.translate(walkDirectionInAir * dt * momentumSpeedDecrease);
        }

        //callback events
        if (closeToTheGround) {
            if (velocity.xz().squareLength() - 0.01f > config.getMaxWalkSpeed() * config.getMaxWalkSpeed()) {
                config.getEventCallback().notifyRunning();
            } else if (velocity.xz().squareLength() - 0.01f > 0.0f) {
                config.getEventCallback().notifyWalking();
            } else {
                config.getEventCallback().notifyNoStepping();
            }
        } else {
            config.getEventCallback().notifyNoStepping();
        }

        //jump
        if (makeJump) [[unlikely]] {
            makeJump = false;
            if (closeToTheGround && !jumping && !crouched) {
                verticalSpeed += config.getJumpSpeed();
                onGround = false;
                jumping = true;
                config.getEventCallback().onStartJumping();
            }
        } else if (onGround && jumping) {
            jumping = false;
        }

        //gravity velocity
        if (!onGround || numberOfHit > 1) {
            if (gravityEnabled) {
                verticalSpeed -= (-physicsWorld.getGravity().Y) * dt;
            }
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

                    if (motionAppliedOnObstacle > motionToPassThroughObstacle * AbstractBody::CCD_MOTION_THRESHOLD_FACTOR) {
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
            Quaternion<float> orientation = Quaternion<float>::lookAt(velocity.normalize()).normalize();
            updatedOrientation = orientation * initialOrientation;
        }

        //save respawn transform
        respawnValues.timeElapse += dt;
        if (respawnValues.timeElapse > SAVE_RESPAWN_TRANSFORM_TIME) {
            respawnValues.respawnTransform = respawnValues.nextRespawnTransform;
            respawnValues.nextRespawnTransform = toStandingBodyTransform(PhysicsTransform(targetPosition, updatedOrientation));
            respawnValues.timeElapse = 0.0f;
        }

        //apply transform on bodies
        applyTransform(PhysicsTransform(targetPosition, updatedOrientation));
    }

    void CharacterController::recoverFromPenetration(float dt) {
        resetSignificantContactValues();

        PhysicsTransform characterTransform = ghostBody->getTransform();
        for (unsigned int subStepIndex = 0; subStepIndex < RECOVER_FACTORS.size(); ++subStepIndex) {
            manifoldResults.clear();
            physicsWorld.getCollisionWorld().getNarrowPhase().processGhostBody(*ghostBody, manifoldResults);

            for (const auto& manifoldResult : manifoldResults) {
                float sign = &manifoldResult.getBody1() == ghostBody.get() ? -1.0f : 1.0f;
                const AbstractBody& otherBody = &manifoldResult.getBody1() == ghostBody.get() ? manifoldResult.getBody2() : manifoldResult.getBody1();
                for (unsigned int i = 0; i < manifoldResult.getNumContactPoints(); ++i) {
                    const ManifoldContactPoint& manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
                    float depth = manifoldContactPoint.getDepth();

                    if (depth < MAX_DEPTH_TO_RECOVER) {
                        Vector3<float> normal = manifoldContactPoint.getNormalFromObject2() * sign;
                        Vector3<float> moveVector = normal * depth * RECOVER_FACTORS[subStepIndex];

                        characterTransform.setPosition(characterTransform.getPosition().translate(moveVector));
                        applyTransform(characterTransform);

                        if (subStepIndex == 0) {
                            saveSignificantContactValues(normal, otherBody);
                        }
                    }
                }
            }
        }

        computeSignificantContactValues(dt);
    }

    void CharacterController::resetSignificantContactValues() {
        significantContactValues.numberOfHit = 0;

        significantContactValues.maxDotProductUpNormalAxis = -std::numeric_limits<float>::max();
        significantContactValues.mostUpVerticalNormal = Vector3<float>();
        significantContactValues.mostUpVerticalBody = nullptr;

        significantContactValues.maxDotProductDownNormalAxis = -std::numeric_limits<float>::max();
        significantContactValues.mostDownVerticalNormal = Vector3<float>();
    }

    void CharacterController::saveSignificantContactValues(const Vector3<float>& normal, const AbstractBody& body) {
        significantContactValues.numberOfHit++;

        float dotProductUpNormalAxis = -normal.Y; //equivalent to: (-normal).dotProduct(Vector3<float>(0.0f, 1.0f, 0.0f));
        if (dotProductUpNormalAxis > significantContactValues.maxDotProductUpNormalAxis) {
            significantContactValues.maxDotProductUpNormalAxis = dotProductUpNormalAxis;
            significantContactValues.mostUpVerticalNormal = -normal;
            significantContactValues.mostUpVerticalBody = &body;
        }

        float dotProductDownNormalAxis = normal.Y; //equivalent to: (-normal).dotProduct(Vector3<float>(0.0f, -1.0f, 0.0f));
        if (dotProductDownNormalAxis > significantContactValues.maxDotProductDownNormalAxis) {
            significantContactValues.maxDotProductDownNormalAxis = dotProductDownNormalAxis;
            significantContactValues.mostDownVerticalNormal = -normal;
        }
    }

    void CharacterController::computeSignificantContactValues(float dt) {
        numberOfHit = significantContactValues.numberOfHit;
        onGround = numberOfHit > 0 && std::acos(significantContactValues.maxDotProductUpNormalAxis) < config.getMaxSlopeInRadian();
        hitRoof = numberOfHit > 0 && std::acos(significantContactValues.maxDotProductDownNormalAxis) < config.getMaxSlopeInRadian();

        if (!onGround) {
            groundBodyId = "";
        } else if (significantContactValues.mostUpVerticalBody != nullptr && groundBodyId != significantContactValues.mostUpVerticalBody->getId()) {
            groundBodyId = significantContactValues.mostUpVerticalBody->getId();
        }

        if (onGround) {
            if (timeInTheAir > 0.05f) {
                config.getEventCallback().onHitGround(timeInTheAir);
            }
            timeInTheAir = 0.0f;
        } else {
            timeInTheAir += dt;
        }
    }

    /**
     * Compute slope based on previous body position.
     * Slope is expressed in percentage. A positive value means that character climb.
     */
    float CharacterController::computeSlope() const {
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

        applyTransform(toActiveBodyTransform(respawnValues.respawnTransform));
        ghostBody->setIsActive(true);
        ccdGhostBody->setIsActive(true);
    }
}
