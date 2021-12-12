#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <map/save/object/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionCompoundShapeReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionCompoundShapeReaderWriter() override = default;

            CollisionShape3D* load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const CollisionShape3D&, UdaWriter&) const override;

        private:
            static void loadTransform(LocalizedCollisionShape&, const UdaChunk*, const UdaParser&);
            static void writeTransform(UdaChunk&, const LocalizedCollisionShape&, UdaWriter&);

            static constexpr char LOCALIZED_SHAPES[] = "localizedShapes";
            static constexpr char LOCALIZED_SHAPE[] = "localizedShape";
            static constexpr char TRANSFORM_TAG[] = "transform";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char COMPOUND_SHAPE_TAG[] = "shape";
    };

}
