#ifndef URCHINENGINE_AMBIENTSOUND_H
#define URCHINENGINE_AMBIENTSOUND_H

#include <string>

#include "sound/Sound.h"

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

#endif
