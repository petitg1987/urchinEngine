#ifndef URCHINENGINE_AICHARACTERCONTROLLER_H
#define URCHINENGINE_AICHARACTERCONTROLLER_H

#include <memory>

#include "AIManager.h"
#include "character/AICharacter.h"
#include "character/AICharacterEventHandler.h"
#include "path/PathRequest.h"

namespace urchin {

    class AICharacterController {
        public:
            AICharacterController(std::shared_ptr<AICharacter> , AIManager*);

            void setupEventHandler(const std::shared_ptr<AICharacterEventHandler>&);

            void moveTo(const Point3<float>&);
            void stopMoving();

            std::shared_ptr<const PathRequest> getPathRequest() const;

            void update();

        private:
            void followPath();

            Point2<float> retrieveNextTarget() const;
            Point2<float> retrieveCharacterPosition() const;

            void computeSteeringMomentum(const Point2<float>&);
            void applyMomentum();
            
            std::shared_ptr<AICharacter> character;
            AIManager* aiManager;
            std::shared_ptr<AICharacterEventHandler> eventHandler;

            Vector2<float> steeringMomentum;

            std::shared_ptr<PathRequest> pathRequest;
            std::vector<PathPoint> pathPoints;
            unsigned int nextPathPointIndex;

    };

}

#endif
