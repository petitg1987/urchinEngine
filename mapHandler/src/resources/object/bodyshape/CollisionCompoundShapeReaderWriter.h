#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionCompoundShapeReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionCompoundShapeReaderWriter() override = default;

            CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const CollisionShape3D&, XmlWriter&) const override;

        private:
            static void loadTransformOn(LocalizedCollisionShape&, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeTransformOn(const std::shared_ptr<XmlChunk>&, const LocalizedCollisionShape&, XmlWriter&);

            static constexpr char LOCALIZED_SHAPES[] = "localizedShapes";
            static constexpr char LOCALIZED_SHAPE[] = "localizedShape";
            static constexpr char TRANSFORM_TAG[] = "transform";
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char COMPOUND_SHAPE_TAG[] = "shape";
    };

}
