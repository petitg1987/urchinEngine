#pragma once

#include <memory>

#include <UrchinSoundEngine.h>
#include <resources/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    /**
    * Retrieve strategy to read/write sound shape
    */
    class SoundShapeReaderWriterRetriever {
        public:
            SoundShapeReaderWriterRetriever() = delete;

            static std::shared_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const XmlChunk*);
            static std::shared_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const SoundShape*);
    };

}
