#ifndef URCHINENGINE_FRUSTUMMODEL_H
#define URCHINENGINE_FRUSTUMMODEL_H

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class FrustumModel : public GeometryModel {
        public:
            explicit FrustumModel(Frustum<float> );

        protected:
            Matrix4<float> retrieveModelMatrix() const override;
            std::vector<Point3<float>> retrieveVertexArray() const override;

            ShapeType getShapeType() const override;

        private:
            Frustum<float> frustum;
    };

}

#endif
