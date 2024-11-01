#pragma once

#include <string>

#include <sound/Sound.h>

namespace urchin {

    /**
     * Non-spatialized sound. Useful for music, UI feedback, etc.
     */
    class GlobalSound final : public Sound {
        public:
            GlobalSound(std::string, SoundCategory, float);

            void initializeSource(ALuint) const override;
            void updateSource(ALuint) override;

            SoundType getSoundType() const override;
    };

}
