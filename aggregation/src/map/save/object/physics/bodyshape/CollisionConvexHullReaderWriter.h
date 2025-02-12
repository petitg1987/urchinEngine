#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <map/save/object/physics/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionConvexHullReaderWriter final : public CollisionShapeReaderWriter {
        public:
            ~CollisionConvexHullReaderWriter() override = default;

            std::unique_ptr<CollisionShape3D> load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const CollisionShape3D&, UdaParser&) const override;

        private:
            static constexpr char POINTS_TAG[] = "points";
            static constexpr char POINT_TAG[] = "point";
    };

}
