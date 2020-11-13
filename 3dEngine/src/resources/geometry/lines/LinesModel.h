#ifndef URCHINENGINE_LINESMODEL_H
#define URCHINENGINE_LINESMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "resources/geometry/GeometryModel.h"

namespace urchin {

    class LinesModel : public GeometryModel {
        public:
            explicit LinesModel(std::vector<Point3<float>>);
            explicit LinesModel(const std::vector<LineSegment3D<float>> &);
            explicit LinesModel(const LineSegment3D<float> &);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            ShapeType getShapeType() const override;

        private:
            std::vector<Point3<float>> linesPoints;
            bool stripLines;
    };

}

#endif
