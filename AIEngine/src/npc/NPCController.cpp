#include "NPCController.h"

namespace urchin
{

    NPCController::NPCController(NonPlayerCharacter *nonPlayerCharacter, AIManager *aiManager) :
            npc(nonPlayerCharacter),
            aiManager(aiManager)
    { //TODO apply gravity on character

    }

    void NPCController::moveTo(const Point3<float> &seekTarget)
    {
        pathRequest = std::make_shared<PathRequest>(npc->getPosition(), seekTarget);
        aiManager->addPathRequest(pathRequest);
    }

    void NPCController::update()
    { //TODO should be automatically call ?: add in processables ?
        if(pathPoints.empty() && pathRequest->isPathReady())
        {
            pathPoints = pathRequest->getPath();
        }

        if(pathPoints.size() >= 2)
        {
            Point3<float> seekTarget = pathPoints.back().getPoint();
            updateSeekForce(seekTarget);
        }

        applyForce();
    }

    void NPCController::updateSeekForce(const Point3<float> &target)
    {
        float maxVelocity = 0.01f;
        Vector3<float> desiredVelocity = npc->getPosition().vector(target).normalize() * maxVelocity;
        Vector3<float> desiredMomentum = desiredVelocity * npc->getMass();
        steeringMomentum = (desiredMomentum - npc->getMomentum());
    }

    void NPCController::applyForce()
    {
        npc->updateMomentum(steeringMomentum);
    }

}
