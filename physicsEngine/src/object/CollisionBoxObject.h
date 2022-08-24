#pragma once

#include <string>
#include <UrchinCommon.h>

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class CollisionBoxObject final : public CollisionConvexObject3D {
        public:
            CollisionBoxObject(float, const Vector3<float>&, const Point3<float>&, const Quaternion<float>&);

            float getHalfSize(unsigned int) const;
            Vector3<float> getHalfSizes() const;
            const Point3<float>& getCenterOfMass() const;
            const Quaternion<float>& getOrientation() const;
            const Vector3<float>& getNormalizedAxis(unsigned int) const;

            CollisionConvexObject3D::ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            OBBox<float> retrieveOBBox() const;
            std::string toString() const override;

        private:
            const OBBox<float> boxObject; //object without margin
    };

}
