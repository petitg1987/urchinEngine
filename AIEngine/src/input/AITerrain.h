#ifndef URCHINENGINE_AITERRAIN_H
#define URCHINENGINE_AITERRAIN_H

#include "input/AIEntity.h"

namespace urchin
{

    class AITerrain : public AIEntity
    {
        public:
            AITerrain(std::string, const Transform<float> &, bool);

            AIEntity::AIEntityType getType() const override;
    };

}

#endif
