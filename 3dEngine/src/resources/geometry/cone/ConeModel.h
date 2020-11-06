#ifndef URCHINENGINE_CONEMODEL_H
#define URCHINENGINE_CONEMODEL_H

#include "UrchinCommon.h"

#include "resources/geometry/GeometryModel.h"

namespace urchin {

    class ConeModel : public GeometryModel {
        public:
            ConeModel(Cone<float> , int);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            void drawGeometry() const override;

        private:
            Cone<float> cone;
            int slices;
    };

}

#endif
