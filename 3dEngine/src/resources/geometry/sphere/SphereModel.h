#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class SphereModel : public GeometryModel {
        public:
            SphereModel(Sphere<float>, unsigned int);
            SphereModel(std::vector<Sphere<float>>, unsigned int);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            ShapeType getShapeType() const override;

        private:
            std::vector<Sphere<float>> spheres;
            unsigned int slices;
    };

}
