#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionCylinderReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionCylinderReaderWriter() override = default;

            CollisionShape3D* loadFrom(const XmlChunk*, const XmlParser&) const override;
            void writeOn(XmlChunk*, const CollisionShape3D&, XmlWriter&) const override;

        private:
            static constexpr char ORIENTATION_TAG[] = "orientation";
            static constexpr char RADIUS_TAG[] = "radius";
            static constexpr char HEIGHT_TAG[] = "height";
            static constexpr char X_VALUE[] = "X";
            static constexpr char Y_VALUE[] = "Y";
            static constexpr char Z_VALUE[] = "Z";
    };

}
