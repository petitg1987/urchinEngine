#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionSphereReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionSphereReaderWriter() override = default;

            CollisionShape3D* loadFrom(const DataChunk*, const DataParser&) const override;
            void writeOn(DataChunk*, const CollisionShape3D&, DataWriter&) const override;

        private:
            static constexpr char RADIUS_TAG[] = "radius";
    };

}
