#ifndef URCHINENGINE_COLLISIONSPHEREREADERWRITER_H
#define URCHINENGINE_COLLISIONSPHEREREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionSphereReaderWriter : public CollisionShapeReaderWriter {
        //XML tags
        #define RADIUS_TAG "radius"

        public:
            ~CollisionSphereReaderWriter() override = default;

            CollisionShape3D *loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const CollisionShape3D*, XmlWriter&) const override;
    };

}

#endif
