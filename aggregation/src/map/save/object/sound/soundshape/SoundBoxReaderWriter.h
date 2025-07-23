#pragma once

#include <memory>
#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

#include "map/save/object/sound/soundshape/SoundShapeReaderWriter.h"

namespace urchin {

    class SoundBoxReaderWriter final : public SoundShapeReaderWriter {
        public:
            ~SoundBoxReaderWriter() override = default;

            std::unique_ptr<SoundShape> load(const UdaChunk*, const UdaParser&) const override;
            void write(UdaChunk&, const SoundShape&, UdaParser&) const override;

        private:
            static constexpr char POSITION_TAG[] = "position";
            static constexpr char HALF_SIZES_TAG[] = "halfSizes";
            static constexpr char ORIENTATION_TAG[] = "orientation";
            static constexpr char MARGIN_TAG[] = "margin";
            static constexpr char AXIS_TAG[] = "axis";
            static constexpr char ANGLE_TAG[] = "angle";
    };

}
