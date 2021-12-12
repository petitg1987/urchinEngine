#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundShapeReaderWriter {
        public:
            static constexpr char TYPE_ATTR[] = "type";
            static constexpr char SPHERE_VALUE[] = "SPHERE";
            static constexpr char BOX_VALUE[] = "BOX";

            virtual ~SoundShapeReaderWriter() = default;

            virtual std::unique_ptr<SoundShape> load(const UdaChunk*, const UdaParser&) const = 0;
            virtual void write(UdaChunk&, const SoundShape&, UdaWriter&) const = 0;
    };

}
