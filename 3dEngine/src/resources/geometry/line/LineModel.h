#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class LineModel final : public GeometryModel {
        public:
            explicit LineModel(const LineSegment3D<float>&, float lineWidth = 0.05f);
            explicit LineModel(std::vector<LineSegment3D<float>>, float lineWidth = 0.05f);

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            ShapeType getShapeType() const override;

        private:
            std::vector<LineSegment3D<float>> lines;
    };

}
