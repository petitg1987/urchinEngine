#pragma once

#include <vector>
#include <UrchinCommon.h>

#include "input/AIEntity.h"

namespace urchin {

    class AITerrain final : public AIEntity {
        public:
            AITerrain(std::string, const Transform<float>&, bool, std::vector<Point3<float>>, unsigned int, unsigned int);

            AIEntityType getType() const override;

            const std::vector<Point3<float>>& getLocalVertices() const;
            unsigned int getXLength() const;
            unsigned int getZLength() const;

        private:
            std::vector<Point3<float>> localVertices;
            unsigned int xLength;
            unsigned int zLength;
    };

}
