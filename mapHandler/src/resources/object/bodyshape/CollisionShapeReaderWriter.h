#ifndef URCHINENGINE_COLLISIONSHAPEREADERWRITER_H
#define URCHINENGINE_COLLISIONSHAPEREADERWRITER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

namespace urchin {

    class CollisionShapeReaderWriter {
        //XML attributes
        #define TYPE_ATTR "type"

        //XML value
        #define SPHERE_VALUE "SPHERE"
        #define CAPSULE_VALUE "CAPSULE"
        #define CYLINDER_VALUE "CYLINDER"
        #define CONE_VALUE "CONE"
        #define BOX_VALUE "BOX"
        #define CONVEX_HULL_VALUE "CONVEX_HULL"
        #define COMPOUND_SHAPE_VALUE "COMPOUND_SHAPE"

        public:
            virtual ~CollisionShapeReaderWriter() = default;

            virtual CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const = 0;
            virtual void writeOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const CollisionShape3D>&, XmlWriter&) const = 0;
    };

}

#endif
