#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <map/save/object/bodyshape/CollisionShapeReaderWriter.h>

namespace urchin {

    class CollisionBoxReaderWriter : public CollisionShapeReaderWriter {
        public:
            ~CollisionBoxReaderWriter() override = default;

            std::unique_ptr<CollisionShape3D> load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const CollisionShape3D&, UdaParser&) const override;

        private:
            static constexpr char HALF_SIZE_TAG[] = "halfSize";
    };

}
