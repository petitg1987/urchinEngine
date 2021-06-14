#include <sound/global/GlobalSound.h>

namespace urchin {

    GlobalSound::GlobalSound(std::string filename) :
            Sound(std::move(filename)) {
        //global sound: always position at 0 distance to listener
        alSourcei(getSourceId(), AL_SOURCE_RELATIVE, true);
        alSource3f(getSourceId(), AL_POSITION, 0.0f, 0.0f, 0.0f);
    }

    Sound::SoundType GlobalSound::getSoundType() const {
        return Sound::GLOBAL;
    }

}
