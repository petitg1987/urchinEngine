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
            void pathFollowing();

            Point2<float> retrieveNextTarget() const;
            Point2<float> retrieveCharacterPosition() const;

            void computeSteeringMomentum(const Point2<float> &);
            void applyMomentum();

            Vector3<float> truncateVector(const Vector3<float> &, float) const;
            Vector2<float> truncateVector(const Vector2<float> &, float) const;

            Point2<float> toPoint2XZ(const Point3<float> &) const;
            Vector2<float> toVector2XZ(const Vector3<float> &) const;

            NonPlayerCharacter *npc;
            AIManager *aiManager;

            Vector2<float> steeringMomentum;

            std::shared_ptr<urchin::PathRequest> pathRequest;
            std::vector<urchin::PathPoint> pathPoints;
            unsigned int nextPathPointIndex;

    };

}

#endif
