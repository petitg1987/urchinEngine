#ifndef URCHINENGINE_SOUNDSHAPEREADERWRITER_H
#define URCHINENGINE_SOUNDSHAPEREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"

namespace urchin {

    class SoundShapeReaderWriter {
        //XML attributes
        #define TYPE_ATTR "type"

        //XML value
        #define SPHERE_VALUE "SPHERE"
        #define BOX_VALUE "BOX"

        public:
            virtual ~SoundShapeReaderWriter() = default;

            virtual SoundShape* loadFrom(const std::shared_ptr<XmlChunk>&, const XmlParser&) const = 0;
            virtual void writeOn(const std::shared_ptr<XmlChunk>&, const SoundShape*, XmlWriter&) const = 0;
    };

}

#endif
