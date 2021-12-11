#pragma once

#include <resources/sound/SoundEntity.h>

namespace urchin {

    class SoundEntityReaderWriter {
        public:
            static std::unique_ptr<SoundEntity> loadFrom(const UdaChunk*, const UdaParser&); //TODO rename methods and similar
            static void writeOn(UdaChunk&, const SoundEntity&, UdaWriter&);

        private:
            static constexpr char NAME_ATTR[] = "name";
            static constexpr char SOUND_TAG[] = "sound";
            static constexpr char SOUND_TRIGGER_TAG[] = "soundTrigger";
    };

}
