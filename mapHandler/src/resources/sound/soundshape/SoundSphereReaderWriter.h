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

            std::unique_ptr<SoundShape> loadFrom(const UdaChunk*, const UdaParser&) const override;
            void writeOn(UdaChunk&, const SoundShape&, UdaWriter&) const override;
    };

}
