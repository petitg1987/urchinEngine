#ifndef URCHINENGINE_COLLISIONCAPSULEREADERWRITER_H
#define URCHINENGINE_COLLISIONCAPSULEREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionCapsuleReaderWriter : public CollisionShapeReaderWriter {
        //XML tags
        #define ORIENTATION_TAG "orientation"
        #define RADIUS_TAG "radius"
        #define CYLINDER_HEIGHT_TAG "cylinderHeight"

        //XML value
        #define X_VALUE "X"
        #define Y_VALUE "Y"
        #define Z_VALUE "Z"

        public:
            ~CollisionCapsuleReaderWriter() override = default;

            CollisionShape3D *loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const override;
            void writeOn(const std::shared_ptr<XmlChunk> &, const CollisionShape3D *, XmlWriter &) const override;
    };

}

#endif
