#ifndef URCHINENGINE_OBBOXMODEL_H
#define URCHINENGINE_OBBOXMODEL_H

#include "UrchinCommon.h"

#include "graphic/geometry/GeometryModel.h"

namespace urchin {

    class OBBoxModel : public GeometryModel {
        public:
            explicit OBBoxModel(OBBox<float> );

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            void drawGeometry() const override;

        private:
            OBBox<float> obbox;
    };

}

#endif
