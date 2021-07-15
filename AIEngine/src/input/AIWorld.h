#pragma once

#include <vector>

#include <input/AIEntity.h>

namespace urchin {

    class AIWorld {
        public:
            AIWorld();

            void addEntity(const std::shared_ptr<AIEntity>&);
            void removeEntity(const std::shared_ptr<AIEntity>&); //TODO ...

            std::vector<std::shared_ptr<AIEntity>> getEntities() const;
            std::vector<std::shared_ptr<AIEntity>> getEntitiesToRemoveAndReset();

        private:
            mutable std::mutex mutex;

            std::vector<std::shared_ptr<AIEntity>> entities;
            std::vector<std::shared_ptr<AIEntity>> entitiesToRemove;
    };

}
