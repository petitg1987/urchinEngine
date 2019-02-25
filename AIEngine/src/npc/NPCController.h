#ifndef URCHINENGINE_NPCCONTROLLER_H
#define URCHINENGINE_NPCCONTROLLER_H

#include "AIManager.h"
#include "npc/NonPlayerCharacter.h"
#include "npc/NPCEventHandler.h"
#include "path/PathRequest.h"

namespace urchin
{

    class NPCController
    {
        public:
            explicit NPCController(NonPlayerCharacter *, AIManager *);

            void setupEventHandler(const std::shared_ptr<NPCEventHandler> &);

            void moveTo(const Point3<float> &);
            void stopMoving();

            void update();

        private:
            void pathFollowing();

            Point2<float> retrieveNextTarget() const;
            Point2<float> retrieveCharacterPosition() const;

            void computeSteeringMomentum(const Point2<float> &);
            void applyMomentum();
            
            NonPlayerCharacter *npc;
            AIManager *aiManager;
            std::shared_ptr<NPCEventHandler> npcEventHandler;

            Vector2<float> steeringMomentum;

            std::shared_ptr<PathRequest> pathRequest;
            std::vector<PathPoint> pathPoints;
            unsigned int nextPathPointIndex;

    };

}

#endif
