#ifndef URCHINENGINE_COLLISIONCONVEXOBJECTPOOL_H
#define URCHINENGINE_COLLISIONCONVEXOBJECTPOOL_H

#include "UrchinCommon.h"
#include "utils/pool/SyncFixedSizePool.h"

namespace urchin {

    class CollisionConvexObject3D;

    class CollisionConvexObjectPool : public Singleton<CollisionConvexObjectPool> {
        public:
            friend class Singleton<CollisionConvexObjectPool>;

            CollisionConvexObjectPool();
            ~CollisionConvexObjectPool() override;

            SyncFixedSizePool<CollisionConvexObject3D> *getObjectsPool();

        private:
            unsigned int maxObjectSize(const std::vector<unsigned int>&);

            SyncFixedSizePool<CollisionConvexObject3D> *objectsPool;
    };

}

#endif
