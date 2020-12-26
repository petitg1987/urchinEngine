#ifndef URCHINENGINE_COLLISIONSHAPEREADERWRITERRETRIEVER_H
#define URCHINENGINE_COLLISIONSHAPEREADERWRITERRETRIEVER_H

#include <memory>
#include "UrchinPhysicsEngine.h"

#include "CollisionShapeReaderWriter.h"

namespace urchin {

    /**
    * Retrieve strategy to read/write collision shape
    */
    class CollisionShapeReaderWriterRetriever {
        public:
            CollisionShapeReaderWriterRetriever() = delete;

            static std::shared_ptr<CollisionShapeReaderWriter> retrieveShapeReaderWriter(const std::shared_ptr<XmlChunk>&);
            static std::shared_ptr<CollisionShapeReaderWriter> retrieveShapeReaderWriter(const std::shared_ptr<const CollisionShape3D>&);
    };

}

#endif
