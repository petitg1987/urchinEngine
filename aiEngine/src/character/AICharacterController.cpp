#include <character/AICharacterController.h>

namespace urchin {

    AICharacterController::AICharacterController(std::unique_ptr<AICharacter> aiCharacter, AIEnvironment& aiEnvironment) :
            aiCharacter(std::move(aiCharacter)),
            aiEnvironment(aiEnvironment),
            eventHandler(nullptr),
            nextPathPointIndex(0) { //see https://gamedevelopment.tutsplus.com/series/understanding-steering-behaviors--gamedev-12732

    }

    void AICharacterController::setupEventHandler(const std::shared_ptr<AICharacterEventHandler>& eventHandler) {
        this->eventHandler = eventHandler;
    }

    void AICharacterController::moveTo(const Point3<float>& seekTarget) {
        stopMoving();
        pathRequest = std::make_shared<PathRequest>(aiCharacter->getPosition(), seekTarget);
        aiEnvironment.addPathRequest(pathRequest);
    }

    void AICharacterController::stopMoving() {
        if (eventHandler) {
            eventHandler->stopMoving();
        }

        nextPathPointIndex = 0;

        if (pathRequest) {
            aiEnvironment.removePathRequest(*pathRequest);
            pathRequest = std::shared_ptr<PathRequest>(nullptr);
        }
        pathPoints.clear();

        aiCharacter->updateVelocity(Vector3(0.0f, 0.0f, 0.0f));
    }

    AICharacter& AICharacterController::getAICharacter() const {
        return *aiCharacter;
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
        return aiCharacter->getPosition().toPoint2XZ();
    }

    void AICharacterController::computeSteeringVelocity(const Point2<float>& target) {
        Vector2<float> desiredVelocity = retrieveCharacterPosition().vector(target).normalize() * aiCharacter->retrieveMaxVelocityInMs();

        steeringVelocity = desiredVelocity - aiCharacter->getVelocity().xz();
        steeringVelocity = steeringVelocity.truncate(aiCharacter->retrieveMaxVelocityInMs());
    }

    void AICharacterController::applyVelocity() const {
        Vector3 steeringVelocity3D(steeringVelocity.X, 0.0f, steeringVelocity.Y);
        Vector3<float> updatedVelocity = aiCharacter->getVelocity() + steeringVelocity3D;
        updatedVelocity = updatedVelocity.truncate(aiCharacter->retrieveMaxVelocityInMs());

        aiCharacter->updateVelocity(updatedVelocity);
    }

}
