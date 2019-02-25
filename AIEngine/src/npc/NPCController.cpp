#include <algorithm>

#include "NPCController.h"

#define CHANGE_PATH_POINT_DISTANCE 0.5f

namespace urchin
{

    NPCController::NPCController(NonPlayerCharacter *nonPlayerCharacter, AIManager *aiManager) :
            npc(nonPlayerCharacter),
            aiManager(aiManager),
            npcEventHandler(nullptr),
            nextPathPointIndex(0)
    { //see https://gamedevelopment.tutsplus.com/series/understanding-steering-behaviors--gamedev-12732

    }

    void NPCController::setupEventHandler(const std::shared_ptr<NPCEventHandler> &npcEventHandler)
    {
        this->npcEventHandler = npcEventHandler;
    }

    void NPCController::moveTo(const Point3<float> &seekTarget)
    {
        stopMoving();
        pathRequest = std::make_shared<PathRequest>(npc->getPosition(), seekTarget);
        aiManager->addPathRequest(pathRequest);
    }

    void NPCController::stopMoving()
    {
        if(npcEventHandler)
        {
            npcEventHandler->stopMoving();
        }

        nextPathPointIndex = 0;

        aiManager->removePathRequest(pathRequest);
        pathRequest = std::shared_ptr<PathRequest>(nullptr);
        pathPoints.clear();

        npc->updateMomentum(Vector3<float>(0.0f, 0.0f, 0.0f));
    }

    void NPCController::update()
    {
        if(pathRequest)
        {
            if (pathPoints.empty() && pathRequest->isPathReady())
            {
                pathPoints = pathRequest->getPath();
                if(!pathPoints.empty() && npcEventHandler)
                {
                    npcEventHandler->startMoving();
                }
            }

            if (!pathPoints.empty())
            {
                followPath();
            }
        }
    }

    void NPCController::followPath()
    {
        Point2<float> nextTarget = retrieveNextTarget();
        if (retrieveCharacterPosition().distance(nextTarget) <= CHANGE_PATH_POINT_DISTANCE)
        {
            nextPathPointIndex++;

            if(nextPathPointIndex >= pathPoints.size())
            { //end of path reached
                stopMoving();
                return;
            }

            nextTarget = retrieveNextTarget();
        }

        computeSteeringMomentum(nextTarget);
        applyMomentum();
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
