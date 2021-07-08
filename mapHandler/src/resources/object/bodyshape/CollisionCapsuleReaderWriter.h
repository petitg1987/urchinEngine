#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionCapsuleReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionCapsuleReaderWriter() override = default;

            CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const CollisionShape3D&, XmlWriter&) const override;

        private:
            static constexpr char ORIENTATION_TAG[] = "orientation";
            static constexpr char RADIUS_TAG[] = "radius";
            static constexpr char CYLINDER_HEIGHT_TAG[] = "cylinderHeight";
            static constexpr char X_VALUE[] = "X";
            static constexpr char Y_VALUE[] = "Y";
            static constexpr char Z_VALUE[] = "Z";
    };

}
