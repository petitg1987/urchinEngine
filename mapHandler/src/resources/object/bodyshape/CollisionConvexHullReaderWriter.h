#ifndef URCHINENGINE_COLLISIONCONVEXHULLREADERWRITER_H
#define URCHINENGINE_COLLISIONCONVEXHULLREADERWRITER_H

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionConvexHullReaderWriter : public CollisionShapeReaderWriter {
        //XML tags
        #define POINTS_TAG "points"
        #define POINT_TAG "point"

        public:
            ~CollisionConvexHullReaderWriter() override = default;

            CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const CollisionShape3D>&, XmlWriter&) const override;
    };

}

#endif
