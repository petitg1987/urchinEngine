#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>
#include "SoundShapeReaderWriter.h"

namespace urchin {

    class SoundSphereReaderWriter : public SoundShapeReaderWriter {
        //XML tags
        static constexpr char POSITION_TAG[] = "position";
        static constexpr char RADIUS_TAG[] = "radius";
        static constexpr char MARGIN_TAG[] = "margin";

        public:
            ~SoundSphereReaderWriter() override = default;

            SoundShape* loadFrom(const XmlChunk*, const XmlParser&) const override;
            void writeOn(XmlChunk*, const SoundShape*, XmlWriter&) const override;
    };

}
