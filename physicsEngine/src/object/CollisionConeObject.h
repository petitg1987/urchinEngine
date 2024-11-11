#pragma once

#include <string>
#include <UrchinCommon.h>

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class CollisionConeObject final : public CollisionConvexObject3D {
        public:
            CollisionConeObject(float, float, float, ConeShape<float>::ConeOrientation, const Point3<float>&, const Quaternion<float>&);

            float getRadius() const;
            float getHeight() const;
            ConeShape<float>::ConeOrientation getConeOrientation() const;
            const Point3<float>& getCenterOfMass() const;
            const Quaternion<float>& getOrientation() const;
            const Vector3<float>& getAxis(unsigned int) const;

            ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            Cone<float> retrieveCone() const;
            std::string toString() const override;

        private:
            const Cone<float> coneObject; //object without margin
    };

}
