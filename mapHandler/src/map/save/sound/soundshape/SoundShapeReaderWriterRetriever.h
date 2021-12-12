#pragma once

#include <memory>

#include <UrchinSoundEngine.h>
#include <map/save/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    /**
    * Retrieve strategy to read/write sound shape
    */
    class SoundShapeReaderWriterRetriever {
        public:
            SoundShapeReaderWriterRetriever() = delete;

            static std::unique_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const UdaChunk*);
            static std::unique_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const SoundShape&);
    };

}
