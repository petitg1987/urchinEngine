#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class CylinderModel : public GeometryModel {
        public:
            CylinderModel(Cylinder<float>, unsigned int);

        protected:
            std::vector<Point3<float>> retrieveVertexArray() const override;
            ShapeType getShapeType() const override;

        private:
            Cylinder<float> cylinder;
            unsigned int sides;
    };

}
