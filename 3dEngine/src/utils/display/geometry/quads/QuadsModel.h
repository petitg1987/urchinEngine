#ifndef URCHINENGINE_QUADSMODEL_H
#define URCHINENGINE_QUADSMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin {

    class QuadsModel : public GeometryModel {
        public:
            explicit QuadsModel(std::vector<Point3<float>>);

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            void drawGeometry() const override;

        private:
            std::vector<Point3<float>> quadsPoints;
    };

}

#endif
