#ifndef URCHINENGINE_AIOBJECT_H
#define URCHINENGINE_AIOBJECT_H

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include "UrchinCommon.h"
#include "input/AIEntity.h"
#include "input/AIShape.h"

namespace urchin
{

    class AIObject : public AIEntity
    {
        public:
            AIObject(std::string, const Transform<float> &, bool, const std::shared_ptr<AIShape> &);
            AIObject(std::string, const Transform<float> &, bool, std::vector<std::shared_ptr<AIShape>>);

            AIEntity::AIEntityType getType() const override;

            const std::vector<std::shared_ptr<AIShape>> &getShapes() const;

        private:
            std::vector<std::shared_ptr<AIShape>> shapes;
    };

}

#endif
