#include "sound/ambient/AmbientSound.h"

namespace urchin {

    AmbientSound::AmbientSound(const std::string &filename) :
        Sound(filename) {
        //ambient sound: always position at 0 distance to listener
        alSourcei(getSourceId(), AL_SOURCE_RELATIVE, true);
        alSource3f(getSourceId(), AL_POSITION, 0.0f, 0.0f, 0.0f);
    }

    Sound::SoundType AmbientSound::getSoundType() const {
        return Sound::AMBIENT;
    }

}
