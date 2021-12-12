#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

#include <map/save/sound/soundshape/SoundShapeReaderWriter.h>

namespace urchin {

    class SoundSphereReaderWriter : public SoundShapeReaderWriter {
        public:
            ~SoundSphereReaderWriter() override = default;

            std::unique_ptr<SoundShape> load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const SoundShape&, UdaWriter&) const override;

        private:
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char RADIUS_TAG[] = "radius";
            static constexpr char MARGIN_TAG[] = "margin";
    };

}
