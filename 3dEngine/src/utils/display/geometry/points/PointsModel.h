#ifndef URCHINENGINE_POINTSMODEL_H
#define URCHINENGINE_POINTSMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin {

    class PointsModel : public GeometryModel {
        public:
            PointsModel(std::vector<Point3<float>>, float);
            PointsModel(const Point3<float> &, float);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            void drawGeometry() const override;

        private:
            std::vector<Point3<float>> points;
            float pointsSize;
    };

}

#endif
