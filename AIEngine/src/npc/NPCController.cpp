#include "NPCController.h"

namespace urchin
{

    NPCController::NPCController(NonPlayerCharacter *nonPlayerCharacter, AIManager *aiManager) :
            npc(nonPlayerCharacter),
            aiManager(aiManager)
    {

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

        applyMomentum();
    }

    void NPCController::updateSeekForce(const Point3<float> &target)
    {
        float maxVelocityKmByHour = 5.0f;
        float maxVelocityMBySec = (maxVelocityKmByHour * 1000.0) / (60.0f * 60.0f);
        Vector3<float> desiredVelocity = npc->getPosition().vector(target).normalize() * maxVelocityMBySec;
        Vector3<float> desiredMomentum = desiredVelocity * npc->getMass();
        steeringMomentum = (desiredMomentum - npc->getMomentum()); //TODO apply a truncate ? see https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-seek--gamedev-849
    }

    void NPCController::applyMomentum()
    {
        npc->updateMomentum(npc->getMomentum() + steeringMomentum); //TODO apply a max speed ? see https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-seek--gamedev-849
    }

}
