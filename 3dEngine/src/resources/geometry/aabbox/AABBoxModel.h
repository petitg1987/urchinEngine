#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class AABBoxModel final : public GeometryModel {
        public:
            explicit AABBoxModel(const AABBox<float>&);
            explicit AABBoxModel(std::vector<AABBox<float>> );

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            ShapeType getShapeType() const override;

        private:
            std::vector<AABBox<float>> aabboxes;
    };

}
