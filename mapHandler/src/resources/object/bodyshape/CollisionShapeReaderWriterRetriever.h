#pragma once

#include <memory>
#include <UrchinPhysicsEngine.h>

#include <resources/object/bodyshape/CollisionShapeReaderWriter.h>

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
