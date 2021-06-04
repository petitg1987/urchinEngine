#pragma once

#include <string>

#include <sound/Sound.h>

namespace urchin {

    /**
     * Ambient sound (unattenuated)
     */
    class AmbientSound : public Sound {
        public:
            explicit AmbientSound(const std::string&);

            Sound::SoundType getSoundType() const override;
    };

}
