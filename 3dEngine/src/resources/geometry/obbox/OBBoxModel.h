#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class OBBoxModel : public GeometryModel {
        public:
            explicit OBBoxModel(OBBox<float> );

        protected:
            std::vector<Point3<float>> retrieveVertexArray() const override;
            ShapeType getShapeType() const override;

        private:
            OBBox<float> obbox;
    };

}
