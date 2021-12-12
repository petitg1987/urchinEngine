#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionConvexHullReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionConvexHullReaderWriter() override = default;

            CollisionShape3D* load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const CollisionShape3D&, UdaWriter&) const override;

        private:
            static constexpr char POINTS_TAG[] = "points";
            static constexpr char POINT_TAG[] = "point";
    };

}
