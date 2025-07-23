#pragma once

#include <vector>
#include <UrchinCommon.h>

#include "resources/geometry/GeometryModel.h"

namespace urchin {

    class TrianglesModel final : public GeometryModel {
        public:
            explicit TrianglesModel(std::vector<Point3<float>>);

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            ShapeType getShapeType() const override;

        private:
            std::vector<Point3<float>> trianglesPoints;
    };

}
