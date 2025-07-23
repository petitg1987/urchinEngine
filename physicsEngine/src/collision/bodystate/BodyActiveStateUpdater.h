#pragma once

#include <vector>

#include "collision/bodystate/IslandContainer.h"
#include "collision/ManifoldResult.h"
#include "body/BodyContainer.h"
#include "body/model/RigidBody.h"

namespace urchin {

    class BodyActiveStateUpdater {
        public:
            explicit BodyActiveStateUpdater(const BodyContainer&);

            void update(const std::vector<ManifoldResult>&);

        private:
            void buildIslands(const std::vector<ManifoldResult>&);
            unsigned int computeNumberElements(const std::vector<IslandElementLink>&, unsigned int) const;
            bool isBodyMoving(const RigidBody*) const;

            void printIslands(const std::vector<IslandElementLink>&) const;

            const BodyContainer& bodyContainer;
            std::vector<IslandElement*> islandElements;
            IslandContainer islandContainer;

            const float squaredLinearSleepingThreshold;
            const float squaredAngularSleepingThreshold;
    };

}
