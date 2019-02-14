#ifndef URCHINENGINE_COLLISIONCONVEXOBJECTPOOL_H
#define URCHINENGINE_COLLISIONCONVEXOBJECTPOOL_H

#include "UrchinCommon.h"
#include "utils/pool/FixedSizePool.h"

namespace urchin
{

    class CollisionConvexObject3D;

    class CollisionConvexObjectPool : public Singleton<CollisionConvexObjectPool>
    {
        public:
            friend class Singleton<CollisionConvexObjectPool>;

            CollisionConvexObjectPool();
            ~CollisionConvexObjectPool() override;

            FixedSizePool<CollisionConvexObject3D> *getObjectsPool();

        private:
            unsigned int maxObjectSize(std::vector<unsigned int>);

            FixedSizePool<CollisionConvexObject3D> *objectsPool;
    };

}

#endif
