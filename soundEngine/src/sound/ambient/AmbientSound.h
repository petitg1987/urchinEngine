#pragma once

#include <string>

#include <sound/Sound.h>

namespace urchin {

    /**
     * Ambient sound (unattenuated)
     */
    class AmbientSound : public Sound {
        public:
            explicit AmbientSound(std::string);

            Sound::SoundType getSoundType() const override;
    };

}
