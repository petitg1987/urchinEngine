#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <map/save/object/physics/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionCompoundShapeReaderWriter final : public CollisionShapeReaderWriter {
        public:
            ~CollisionCompoundShapeReaderWriter() override = default;

            std::unique_ptr<CollisionShape3D> load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const CollisionShape3D&, UdaParser&) const override;

        private:
            static void loadTransform(LocalizedCollisionShape&, const UdaChunk*, const UdaParser&);
            static void writeTransform(UdaChunk&, const LocalizedCollisionShape&, UdaParser&);

            static constexpr char LOCALIZED_SHAPES[] = "localizedShapes";
            static constexpr char LOCALIZED_SHAPE[] = "localizedShape";
            static constexpr char TRANSFORM_TAG[] = "transform";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char COMPOUND_SHAPE_TAG[] = "shape";
    };

}
