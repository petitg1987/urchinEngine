#include <algorithm>

#include "NPCController.h"

namespace urchin
{

    NPCController::NPCController(NonPlayerCharacter *nonPlayerCharacter, AIManager *aiManager) :
            npc(nonPlayerCharacter),
            aiManager(aiManager),
            nextPathPointIndex(0)
    {

    }

    void NPCController::moveTo(const Point3<float> &seekTarget)
    {
        pathRequest = std::make_shared<PathRequest>(npc->getPosition(), seekTarget);
        aiManager->addPathRequest(pathRequest);
    }

    void NPCController::update()
    {
        if(pathPoints.empty() && pathRequest->isPathReady())
        {
            pathPoints = pathRequest->getPath();
        }

        if(pathPoints.size() >= 2)
        {
            pathFollowing();
        }

        applyMomentum();
    }

    void NPCController::pathFollowing()
    {
        Point2<float> nextTarget = retrieveNextTarget();
        if (retrieveCharacterPosition().distance(nextTarget) <= 0.5f) //TODO define distant in #define
        {
            nextPathPointIndex = std::min(nextPathPointIndex + 1, static_cast<unsigned int>(pathPoints.size() - 1));
            nextTarget = retrieveNextTarget();
        }

        computeSteeringMomentum(nextTarget);
    }

    Point2<float> NPCController::retrieveNextTarget() const
    {
        return pathPoints[nextPathPointIndex].getPoint().toPoint2XZ();
    }

    Point2<float> NPCController::retrieveCharacterPosition() const
    {
        return npc->getPosition().toPoint2XZ();
    }

    void NPCController::computeSteeringMomentum(const Point2<float> &target)
    {
        Vector2<float> desiredVelocity = retrieveCharacterPosition().vector(target).normalize() * npc->retrieveMaxVelocityInMs();
        Vector2<float> desiredMomentum = desiredVelocity * npc->getMass();

        steeringMomentum = desiredMomentum - npc->getMomentum().toVector2XZ();
        steeringMomentum = steeringMomentum.truncate(npc->retrieveMaxMomentum());
    }

    void NPCController::applyMomentum()
    {
        Vector3<float> steeringMomentum3D(steeringMomentum.X, 0.0f, steeringMomentum.Y);
        Vector3<float> updatedMomentum = npc->getMomentum() + steeringMomentum3D;
        updatedMomentum = updatedMomentum.truncate(npc->retrieveMaxMomentum());

        npc->updateMomentum(updatedMomentum);
    }

}
