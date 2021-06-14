#pragma once

#include <string>

#include <sound/Sound.h>

namespace urchin {

    /**
     * Non-spatialized sound. Useful for music, UI feedback, etc.
     */
    class GlobalSound : public Sound {
        public:
            GlobalSound(std::string, SoundCategory);

            Sound::SoundType getSoundType() const override;
    };

}
