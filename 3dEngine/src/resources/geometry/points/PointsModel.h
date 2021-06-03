#ifndef URCHINENGINE_POINTSMODEL_H
#define URCHINENGINE_POINTSMODEL_H

#include <vector>
#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class PointsModel : public GeometryModel {
        public:
            explicit PointsModel(std::vector<Point3<float>>);
            explicit PointsModel(const Point3<float>&);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            ShapeType getShapeType() const override;

        private:
            std::vector<Point3<float>> points;
    };

}

#endif
