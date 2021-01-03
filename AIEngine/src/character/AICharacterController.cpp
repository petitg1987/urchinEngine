#include "AICharacterController.h"

#define CHANGE_PATH_POINT_DISTANCE 0.4f

namespace urchin {

    AICharacterController::AICharacterController(std::shared_ptr<AICharacter> character, AIManager* aiManager) :
            character(std::move(character)),
            aiManager(aiManager),
            eventHandler(nullptr),
            nextPathPointIndex(0) { //see https://gamedevelopment.tutsplus.com/series/understanding-steering-behaviors--gamedev-12732

    }

    void AICharacterController::setupEventHandler(const std::shared_ptr<AICharacterEventHandler>& eventHandler) {
        this->eventHandler = eventHandler;
    }

    void AICharacterController::moveTo(const Point3<float>& seekTarget) {
        stopMoving();
        pathRequest = std::make_shared<PathRequest>(character->getPosition(), seekTarget);
        aiManager->addPathRequest(pathRequest);
    }

    void AICharacterController::stopMoving() {
        if (eventHandler) {
            eventHandler->stopMoving();
        }

        nextPathPointIndex = 0;

        aiManager->removePathRequest(pathRequest);
        pathRequest = std::shared_ptr<PathRequest>(nullptr);
        pathPoints.clear();

        character->updateMomentum(Vector3<float>(0.0f, 0.0f, 0.0f));
    }

    std::shared_ptr<const PathRequest> AICharacterController::getPathRequest() const {
        return pathRequest;
    }

    void AICharacterController::update() {
        if (pathRequest) {
            if (pathPoints.empty() && pathRequest->isPathReady()) {
                pathPoints = pathRequest->getPath();
                if (!pathPoints.empty() && eventHandler) {
                    eventHandler->startMoving();
                }
            }

            if (!pathPoints.empty()) {
                followPath();
            }
        }
    }

    void AICharacterController::followPath() {
        Point2<float> nextTarget = retrieveNextTarget();
        if (retrieveCharacterPosition().distance(nextTarget) <= CHANGE_PATH_POINT_DISTANCE) {
            nextPathPointIndex++;

            if (nextPathPointIndex >= pathPoints.size()) { //end of path reached
                stopMoving();
                return;
            }

            nextTarget = retrieveNextTarget();
        }

        computeSteeringMomentum(nextTarget);
        applyMomentum();
    }

    Point2<float> AICharacterController::retrieveNextTarget() const {
        return pathPoints[nextPathPointIndex].getPoint().toPoint2XZ();
    }

    Point2<float> AICharacterController::retrieveCharacterPosition() const {
        return character->getPosition().toPoint2XZ();
    }

    void AICharacterController::computeSteeringMomentum(const Point2<float>& target) {
        Vector2<float> desiredVelocity = retrieveCharacterPosition().vector(target).normalize() * character->retrieveMaxVelocityInMs();
        Vector2<float> desiredMomentum = desiredVelocity * character->getMass();

        steeringMomentum = desiredMomentum - character->getMomentum().xz();
        steeringMomentum = steeringMomentum.truncate(character->retrieveMaxMomentum());
    }

    void AICharacterController::applyMomentum() {
        Vector3<float> steeringMomentum3D(steeringMomentum.X, 0.0f, steeringMomentum.Y);
        Vector3<float> updatedMomentum = character->getMomentum() + steeringMomentum3D;
        updatedMomentum = updatedMomentum.truncate(character->retrieveMaxMomentum());

        character->updateMomentum(updatedMomentum);
    }

}
