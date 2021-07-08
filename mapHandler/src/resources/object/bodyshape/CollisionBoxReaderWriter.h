#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionBoxReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionBoxReaderWriter() override = default;

            CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const CollisionShape3D&, XmlWriter&) const override;

        private:
            static constexpr char HALF_SIZE_TAG[] = "halfSize";
    };

}
