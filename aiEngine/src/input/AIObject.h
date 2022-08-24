#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include <UrchinCommon.h>
#include <input/AIEntity.h>
#include <input/AIShape.h>

namespace urchin {

    class AIObject final : public AIEntity {
        public:
            AIObject(std::string, const Transform<float>&, bool, std::unique_ptr<AIShape>);
            AIObject(std::string, const Transform<float>&, bool, std::vector<std::unique_ptr<AIShape>>);

            AIEntity::AIEntityType getType() const override;

            const std::vector<std::unique_ptr<AIShape>>& getShapes() const;

        private:
            std::vector<std::unique_ptr<AIShape>> shapes;
    };

}
