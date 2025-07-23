#pragma once

#include <memory>
#include <UrchinPhysicsEngine.h>

#include "map/save/object/physics/bodyshape/CollisionShapeReaderWriter.h"

namespace urchin {

    /**
    * Retrieve strategy to read/write collision shape
    */
    class CollisionShapeReaderWriterRetriever {
        public:
            CollisionShapeReaderWriterRetriever() = delete;

            static std::unique_ptr<CollisionShapeReaderWriter> retrieveShapeReaderWriter(const UdaChunk*);
            static std::unique_ptr<CollisionShapeReaderWriter> retrieveShapeReaderWriter(const CollisionShape3D&);
    };

}
