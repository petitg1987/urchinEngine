#ifndef URCHINENGINE_CYLINDERMODEL_H
#define URCHINENGINE_CYLINDERMODEL_H

#include "UrchinCommon.h"

#include "resources/geometry/GeometryModel.h"

namespace urchin {

    class CylinderModel : public GeometryModel {
        public:
            CylinderModel(Cylinder<float> , int);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            ShapeType getShapeType() const override;

        private:
            Cylinder<float> cylinder;
            int sides;
    };

}

#endif
