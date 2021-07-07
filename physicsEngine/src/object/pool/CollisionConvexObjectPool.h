#pragma once

#include <UrchinCommon.h>
#include <utils/pool/SyncFixedSizePool.h>

namespace urchin {

    class CollisionConvexObject3D;

    class CollisionConvexObjectPool : public Singleton<CollisionConvexObjectPool> { //TODO several instances could be created by 2 different threads !
        public:
            friend class Singleton<CollisionConvexObjectPool>;

            CollisionConvexObjectPool();
            ~CollisionConvexObjectPool() override;

            SyncFixedSizePool<CollisionConvexObject3D>* getObjectsPool();

        private:
            unsigned int maxObjectSize(const std::vector<unsigned int>&) const;

            SyncFixedSizePool<CollisionConvexObject3D>* objectsPool;
    };

}
