#pragma once

#include <UrchinCommon.h>
#include <utils/pool/SyncFixedSizePool.h>

namespace urchin {

    class CollisionConvexObject3D;

    class CollisionConvexObjectPool : public ThreadSafeSingleton<CollisionConvexObjectPool> {
        public:
            friend class ThreadSafeSingleton<CollisionConvexObjectPool>;

            CollisionConvexObjectPool();
            ~CollisionConvexObjectPool() override = default;

            SyncFixedSizePool<CollisionConvexObject3D>& getObjectsPool();

        private:
            unsigned int maxObjectSize(const std::vector<unsigned int>&) const;

            std::unique_ptr<SyncFixedSizePool<CollisionConvexObject3D>> objectsPool;
    };

}
