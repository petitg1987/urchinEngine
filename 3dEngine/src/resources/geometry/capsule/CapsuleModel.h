#pragma once

#include <UrchinCommon.h>

#include <resources/geometry/GeometryModel.h>

namespace urchin {

    class CapsuleModel final : public GeometryModel {
        public:
            CapsuleModel(Capsule<float>, unsigned int, unsigned int);

        protected:
            std::vector<Point3<float>> retrieveVertexArray(std::vector<uint32_t>&) const override;
            ShapeType getShapeType() const override;

        private:
            void fillWithCaps(std::vector<Point3<float>>&, const Quaternion<float>&) const;

            Capsule<float> capsule;
            unsigned int sides;
            unsigned int slices;
    };

}
