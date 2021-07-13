#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionConvexHullReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionConvexHullReaderWriter() override = default;

            CollisionShape3D* loadFrom(const UdaChunk*, const DataParser&) const override;
            void writeOn(UdaChunk&, const CollisionShape3D&, UdaWriter&) const override;

        private:
            static constexpr char POINTS_TAG[] = "points";
            static constexpr char POINT_TAG[] = "point";
    };

}
