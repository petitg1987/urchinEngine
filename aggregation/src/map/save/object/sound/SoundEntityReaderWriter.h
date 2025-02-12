#pragma once

#include <model/SoundEntity.h>

namespace urchin {

    class SoundEntityReaderWriter {
        public:
            static std::unique_ptr<SoundEntity> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const SoundEntity&, UdaParser&);

        private:
            static constexpr char NAME_ATTR[] = "name";
            static constexpr char SOUND_TAG[] = "sound";
            static constexpr char SOUND_TRIGGER_TAG[] = "soundTrigger";
    };

}
