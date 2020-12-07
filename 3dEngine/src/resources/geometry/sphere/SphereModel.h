#ifndef URCHINENGINE_SPHEREMODEL_H
#define URCHINENGINE_SPHEREMODEL_H

#include "UrchinCommon.h"

#include "resources/geometry/GeometryModel.h"

namespace urchin {

    class SphereModel : public GeometryModel {
        public:
            SphereModel(Sphere<float>, unsigned int);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            ShapeType getShapeType() const override;

        private:
            Sphere<float> sphere;
            unsigned int slices;
    };

}

#endif
