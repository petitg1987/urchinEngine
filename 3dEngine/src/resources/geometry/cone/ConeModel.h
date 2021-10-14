#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class ConeModel : public GeometryModel {
        public:
            ConeModel(Cone<float>, unsigned int);

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            ShapeType getShapeType() const override;

        private:
            Cone<float> cone;
            unsigned int slices;
    };

}
