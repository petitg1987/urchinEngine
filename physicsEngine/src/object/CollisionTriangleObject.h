#pragma once

#include <string>
#include <UrchinCommon.h>

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class CollisionTriangleObject final : public CollisionConvexObject3D {
        public:
            CollisionTriangleObject(float, const Point3<float>&, const Point3<float>&, const Point3<float>&);

            const Point3<float>& getPoint(unsigned int) const;

            ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            Triangle3D<float> retrieveTriangle() const;
            std::string toString() const override;

        private:
            const Triangle3D<float> triangleObject; //object without margin
    };

}
