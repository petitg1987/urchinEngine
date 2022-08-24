#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <map/save/object/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionCapsuleReaderWriter final : public CollisionShapeReaderWriter {
        public:
            ~CollisionCapsuleReaderWriter() override = default;

            std::unique_ptr<CollisionShape3D> load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const CollisionShape3D&, UdaParser&) const override;

        private:
            static constexpr char ORIENTATION_TAG[] = "orientation";
            static constexpr char RADIUS_TAG[] = "radius";
            static constexpr char CYLINDER_HEIGHT_TAG[] = "cylinderHeight";
            static constexpr char X_VALUE[] = "X";
            static constexpr char Y_VALUE[] = "Y";
            static constexpr char Z_VALUE[] = "Z";
    };

}
