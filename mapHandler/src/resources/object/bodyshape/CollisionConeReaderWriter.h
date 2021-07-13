#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionConeReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionConeReaderWriter() override = default;

            CollisionShape3D* loadFrom(const DataChunk*, const DataParser&) const override;
            void writeOn(DataChunk&, const CollisionShape3D&, UdaWriter&) const override;

        private:
            static constexpr char ORIENTATION_TAG[] = "orientation";
            static constexpr char RADIUS_TAG[] = "radius";
            static constexpr char HEIGHT_TAG[] = "height";
            static constexpr char X_POSITIVE_VALUE[] = "XPositive";
            static constexpr char X_NEGATIVE_VALUE[] = "XNegative";
            static constexpr char Y_POSITIVE_VALUE[] = "YPositive";
            static constexpr char Y_NEGATIVE_VALUE[] = "YNegative";
            static constexpr char Z_POSITIVE_VALUE[] = "ZPositive";
            static constexpr char Z_NEGATIVE_VALUE[] = "ZNegative";
    };

}
