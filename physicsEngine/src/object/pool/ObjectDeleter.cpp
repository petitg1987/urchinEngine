#include <object/pool/ObjectDeleter.h>
#include <object/CollisionConvexObject3D.h>
#include <object/pool/CollisionConvexObjectPool.h>

namespace urchin {
    
    void ObjectDeleter::operator()(CollisionConvexObject3D *const collisionAlgorithm) {
        CollisionConvexObjectPool::instance()->getObjectsPool().deallocate(collisionAlgorithm);
    }

}
