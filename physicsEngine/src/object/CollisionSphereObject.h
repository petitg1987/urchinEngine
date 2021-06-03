#ifndef URCHINENGINE_COLLISIONSPHEREOBJECT_H
#define URCHINENGINE_COLLISIONSPHEREOBJECT_H

#include <string>
#include <UrchinCommon.h>

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class CollisionSphereObject : public CollisionConvexObject3D {
        public:
            CollisionSphereObject(float, const Point3<float>&);

            float getRadius() const;
            const Point3<float>& getCenterOfMass() const;

            CollisionConvexObject3D::ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            Sphere<float> retrieveSphere() const;
            std::string toString() const override;

        private:
            const Sphere<float> sphereObject; //object without margin
    };

}

#endif
