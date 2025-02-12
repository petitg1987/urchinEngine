#pragma once

#include <memory>
#include <UrchinSoundEngine.h>

namespace urchin {

    class SoundComponentReaderWriter {
        public:
            static std::unique_ptr<SoundComponent> load(const UdaChunk*, const UdaParser&);
            static void write(UdaChunk&, const SoundComponent&, UdaParser&);

        private:
            static constexpr char SOUND_TAG[] = "sound";
            static constexpr char SOUND_TRIGGER_TAG[] = "soundTrigger";
    };

}
