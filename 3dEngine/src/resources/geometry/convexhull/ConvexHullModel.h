#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class ConvexHullModel : public GeometryModel {
        public:
            explicit ConvexHullModel(ConvexHull3D<float> );

        protected:
            std::vector<Point3<float>> retrieveVertexArray() const override;
            ShapeType getShapeType() const override;

        private:
            ConvexHull3D<float> convexHull;
    };

}
