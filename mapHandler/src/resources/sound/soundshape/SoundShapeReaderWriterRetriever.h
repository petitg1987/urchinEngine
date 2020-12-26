#ifndef URCHINENGINE_SOUNDSHAPEREADERWRITERRETRIEVER_H
#define URCHINENGINE_SOUNDSHAPEREADERWRITERRETRIEVER_H

#include <memory>

#include "UrchinSoundEngine.h"
#include "resources/sound/soundshape/SoundShapeReaderWriter.h"

namespace urchin {

    /**
    * Retrieve strategy to read/write sound shape
    */
    class SoundShapeReaderWriterRetriever {
        public:
            std::shared_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const std::shared_ptr<XmlChunk>&);
            std::shared_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const SoundShape*);
    };

}

#endif
