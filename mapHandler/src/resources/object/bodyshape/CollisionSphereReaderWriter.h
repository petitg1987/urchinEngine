#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionSphereReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionSphereReaderWriter() override = default;

            CollisionShape3D* loadFrom(const XmlChunk*, const DataParser&) const override;
            void writeOn(XmlChunk*, const CollisionShape3D&, XmlWriter&) const override;

        private:
            static constexpr char RADIUS_TAG[] = "radius";
    };

}
