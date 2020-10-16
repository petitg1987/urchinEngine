#ifndef URCHINENGINE_AABBOXMODEL_H
#define URCHINENGINE_AABBOXMODEL_H

#include <vector>
#include "UrchinCommon.h"

#include "utils/display/geometry/GeometryModel.h"

namespace urchin {

    class AABBoxModel : public GeometryModel {
        public:
            explicit AABBoxModel(const AABBox<float> &);
            explicit AABBoxModel(std::vector<AABBox<float>> );

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            void drawGeometry() const override;

        private:
            std::vector<AABBox<float>> aabboxes;
    };

}

#endif
