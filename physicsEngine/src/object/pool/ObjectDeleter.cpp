#include <object/pool/ObjectDeleter.h>
#include <object/CollisionConvexObject3D.h>
#include <object/pool/CollisionConvexObjectPool.h>

namespace urchin {
    
    void ObjectDeleter::operator()(CollisionConvexObject3D *const collisionAlgorithm) {
        static CollisionConvexObjectPool* convexObjectPool = CollisionConvexObjectPool::instance(); //static to retrieve thread safe instance only once
        convexObjectPool->getObjectsPool()->deallocate(collisionAlgorithm);
    }

}
