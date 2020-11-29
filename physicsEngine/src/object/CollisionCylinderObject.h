#ifndef URCHINENGINE_COLLISIONCYLINDEROBJECT_H
#define URCHINENGINE_COLLISIONCYLINDEROBJECT_H

#include <string>
#include "UrchinCommon.h"

#include "object/CollisionConvexObject3D.h"

namespace urchin {

    class CollisionCylinderObject : public CollisionConvexObject3D {
        public:
            CollisionCylinderObject(float, float, float, typename CylinderShape<float>::CylinderOrientation, const Point3<float>&, const Quaternion<float>&);

            float getRadius() const;
            float getHeight() const;
            typename CylinderShape<float>::CylinderOrientation getCylinderOrientation() const;
            const Point3<float>& getCenterOfMass() const;
            const Quaternion<float>& getOrientation() const;
            const Vector3<float>& getAxis(unsigned int) const;

            CollisionConvexObject3D::ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            Cylinder<float> retrieveCylinder() const;
            std::string toString() const override;

        private:
            const Cylinder<float> cylinderObject; //object without margin
    };

}

#endif
