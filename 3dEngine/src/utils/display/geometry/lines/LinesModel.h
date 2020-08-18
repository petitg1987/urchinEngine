#ifndef URCHINENGINE_LINESMODEL_H
#define URCHINENGINE_LINESMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin
{

    class LinesModel : public GeometryModel
    {
        public:
            LinesModel(std::vector<Point3<float>>, float);
            LinesModel(const std::vector<LineSegment3D<float>> &, float);
            LinesModel(const LineSegment3D<float> &, float);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            void drawGeometry() const override;

        private:
            std::vector<Point3<float>> linesPoints;
            bool stripLines;
            float linesSize;
    };

}

#endif
