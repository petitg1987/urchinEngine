#ifndef URCHINENGINE_COLLISIONOBJECTWRAPPER_H
#define URCHINENGINE_COLLISIONOBJECTWRAPPER_H

#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

    /**
    * Wrapper for object used in collision algorithms
    */
    class CollisionObjectWrapper
    {
        public:
            CollisionObjectWrapper(const CollisionShape3D &, const PhysicsTransform &);

            const CollisionShape3D &getShape() const;
            const PhysicsTransform &getShapeWorldTransform() const;

        private:
            const CollisionShape3D &shape;
            const PhysicsTransform &shapeWorldTransform;
    };

}

#endif
