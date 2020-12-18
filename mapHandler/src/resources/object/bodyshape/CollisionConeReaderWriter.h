#ifndef URCHINENGINE_COLLISIONCONEREADERWRITER_H
#define URCHINENGINE_COLLISIONCONEREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionConeReaderWriter : public CollisionShapeReaderWriter {
        //XML tags
        #define ORIENTATION_TAG "orientation"
        #define RADIUS_TAG "radius"
        #define HEIGHT_TAG "height"

        //XML value
        #define X_POSITIVE_VALUE "XPositive"
        #define X_NEGATIVE_VALUE "XNegative"
        #define Y_POSITIVE_VALUE "YPositive"
        #define Y_NEGATIVE_VALUE "YNegative"
        #define Z_POSITIVE_VALUE "ZPositive"
        #define Z_NEGATIVE_VALUE "ZNegative"

        public:
            ~CollisionConeReaderWriter() override = default;

            CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const CollisionShape3D>&, XmlWriter&) const override;
    };

}

#endif
