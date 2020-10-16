#ifndef URCHINENGINE_SOUNDSPHEREREADERWRITER_H
#define URCHINENGINE_SOUNDSPHEREREADERWRITER_H

#include <memory>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"
#include "SoundShapeReaderWriter.h"

namespace urchin {

    class SoundSphereReaderWriter : public SoundShapeReaderWriter {
        //XML tags
        #define POSITION_TAG "position"
        #define RADIUS_TAG "radius"
        #define MARGIN_TAG "margin"

        public:
            ~SoundSphereReaderWriter() override = default;

            SoundShape *loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &) const override;
            void writeOn(const std::shared_ptr<XmlChunk> &, const SoundShape *, XmlWriter &) const override;
    };

}

#endif
