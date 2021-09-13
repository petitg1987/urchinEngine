#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <resources/model/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionSphereReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionSphereReaderWriter() override = default;

            CollisionShape3D* loadFrom(const UdaChunk*, const UdaParser&) const override;
            void writeOn(UdaChunk&, const CollisionShape3D&, UdaWriter&) const override;

        private:
            static constexpr char RADIUS_TAG[] = "radius";
    };

}
