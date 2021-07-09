#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionCompoundShapeReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionCompoundShapeReaderWriter() override = default;

            CollisionShape3D* loadFrom(const XmlChunk*, const DataParser&) const override;
            void writeOn(XmlChunk*, const CollisionShape3D&, DataWriter&) const override;

        private:
            static void loadTransformOn(LocalizedCollisionShape&, const XmlChunk*, const DataParser&);
            static void writeTransformOn(const XmlChunk*, const LocalizedCollisionShape&, DataWriter&);

            static constexpr char LOCALIZED_SHAPES[] = "localizedShapes";
            static constexpr char LOCALIZED_SHAPE[] = "localizedShape";
            static constexpr char TRANSFORM_TAG[] = "transform";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char COMPOUND_SHAPE_TAG[] = "shape";
    };

}
