#pragma once

#include <UrchinCommon.h>

#include "utils/pool/SyncFixedSizePool.h"

namespace urchin {

    class CollisionConvexObject3D;

    class CollisionConvexObjectPool {
        public:
            static CollisionConvexObjectPool& instance();

            SyncFixedSizePool<CollisionConvexObject3D>& getObjectsPool() const;

        private:
            CollisionConvexObjectPool();

            unsigned int maxObjectSize(const std::vector<unsigned int>&) const;

            std::unique_ptr<SyncFixedSizePool<CollisionConvexObject3D>> objectsPool;
    };

}
