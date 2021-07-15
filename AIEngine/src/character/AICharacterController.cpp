#include <character/AICharacterController.h>

namespace urchin {

    //static
    constexpr float AICharacterController::CHANGE_PATH_POINT_DISTANCE = 0.4f;

    AICharacterController::AICharacterController(std::shared_ptr<AICharacter> character, AIManager& aiManager) :
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
        aiManager.addPathRequest(pathRequest);
    }

    void AICharacterController::stopMoving() {
        if (eventHandler) {
            eventHandler->stopMoving();
        }

        nextPathPointIndex = 0;

        if (pathRequest) {
            aiManager.removePathRequest(*pathRequest);
            pathRequest = std::shared_ptr<PathRequest>(nullptr);
        }
        pathPoints.clear();

        character->updateVelocity(Vector3<float>(0.0f, 0.0f, 0.0f));
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

        computeSteeringVelocity(nextTarget);
        applyVelocity();
    }

    Point2<float> AICharacterController::retrieveNextTarget() const {
        return pathPoints[nextPathPointIndex].getPoint().toPoint2XZ();
    }

    Point2<float> AICharacterController::retrieveCharacterPosition() const {
        return character->getPosition().toPoint2XZ();
    }

    void AICharacterController::computeSteeringVelocity(const Point2<float>& target) {
        Vector2<float> desiredVelocity = retrieveCharacterPosition().vector(target).normalize() * character->retrieveMaxVelocityInMs();

        steeringVelocity = desiredVelocity - character->getVelocity().xz();
        steeringVelocity = steeringVelocity.truncate(character->retrieveMaxVelocityInMs());
    }

    void AICharacterController::applyVelocity() {
        Vector3<float> steeringVelocity3D(steeringVelocity.X, 0.0f, steeringVelocity.Y);
        Vector3<float> updatedVelocity = character->getVelocity() + steeringVelocity3D;
        updatedVelocity = updatedVelocity.truncate(character->retrieveMaxVelocityInMs());

        character->updateVelocity(updatedVelocity);
    }

}
