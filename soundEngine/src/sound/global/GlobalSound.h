#pragma once

#include <string>

#include <sound/Sound.h>

namespace urchin {

    /**
     * Non-spatialized sound. Useful for music, UI feedback, etc.
     */
    class GlobalSound : public Sound {
        public:
            explicit GlobalSound(std::string);

            Sound::SoundType getSoundType() const override;
    };

}
