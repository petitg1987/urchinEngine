#ifndef URCHINENGINE_NPCCONTROLLER_H
#define URCHINENGINE_NPCCONTROLLER_H

#include "npc/NonPlayerCharacter.h"
#include "AIManager.h"
#include "path/PathRequest.h"

namespace urchin
{

    class NPCController
    {
        public:
            explicit NPCController(NonPlayerCharacter *, AIManager *);

            void moveTo(const Point3<float> &);

            void update();

        private:
            void updateSeekForce(const Point3<float> &);
            void applyForce();

            NonPlayerCharacter *npc;
            AIManager *aiManager;

            Vector3<float> steeringMomentum;

            std::shared_ptr<urchin::PathRequest> pathRequest;
            std::vector<urchin::PathPoint> pathPoints;

    };

}

#endif
