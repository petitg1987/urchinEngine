#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class SphereModel final : public GeometryModel {
        public:
            SphereModel(Sphere<float>, unsigned int);
            SphereModel(std::vector<Sphere<float>>, unsigned int);

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            ShapeType getShapeType() const override;

        private:
            std::vector<Sphere<float>> spheres;
            unsigned int slices;
    };

}
