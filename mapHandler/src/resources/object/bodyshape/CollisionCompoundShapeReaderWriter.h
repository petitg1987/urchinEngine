#ifndef URCHINENGINE_COLLISIONCOMPOUNDSHAPEREADERWRITER_H
#define URCHINENGINE_COLLISIONCOMPOUNDSHAPEREADERWRITER_H

#include <memory>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    class CollisionCompoundShapeReaderWriter : public CollisionShapeReaderWriter {
        //XML tags
        #define LOCALIZED_SHAPES "localizedShapes"
        #define LOCALIZED_SHAPE "localizedShape"
        #define TRANSFORM_TAG "transform"
        #define POSITION_TAG "position"
        #define SHAPE "shape"

        public:
            ~CollisionCompoundShapeReaderWriter() override = default;

            CollisionShape3D* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const override;
            void writeOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const CollisionShape3D>&, XmlWriter&) const override;

        private:
            static void loadTransformOn(const std::shared_ptr<LocalizedCollisionShape>&, const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeTransformOn(const std::shared_ptr<XmlChunk>&, const std::shared_ptr<const LocalizedCollisionShape>&, XmlWriter&);

    };

}

#endif
