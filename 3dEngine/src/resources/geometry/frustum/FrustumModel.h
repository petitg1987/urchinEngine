#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class FrustumModel final : public GeometryModel {
        public:
            explicit FrustumModel(Frustum<float>);

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            std::vector<LineSegment3D<float>> retrieveWireframeLines() const override;
            ShapeType getShapeType() const override;

        private:
            Frustum<float> frustum;
    };

}
