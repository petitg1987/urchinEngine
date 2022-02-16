#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include <object/CollisionConvexObject3D.h>

namespace urchin {

    class CollisionConvexHullObject : public CollisionConvexObject3D {
        public:
            CollisionConvexHullObject(float, const std::vector<Point3<float>>&, const std::vector<Point3<float>>&);
            CollisionConvexHullObject(float, std::unique_ptr<ConvexHull3D<float>>, std::unique_ptr<ConvexHull3D<float>>);
            explicit CollisionConvexHullObject(std::unique_ptr<ConvexHull3D<float>>);

            ConvexHull3D<float>& getConvexHullWithoutMargin() const;
            ConvexHull3D<float>& getConvexHullWithMargin() const;

            CollisionConvexObject3D::ObjectType getObjectType() const override;
            Point3<float> getSupportPoint(const Vector3<float>&, bool) const override;

            std::string toString() const override;

        private:
            std::unique_ptr<ConvexHull3D<float>> convexHullObjectWithMargin;
            std::unique_ptr<ConvexHull3D<float>> convexHullObjectWithoutMargin;
    };

}
