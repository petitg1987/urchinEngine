#include <sound/global/GlobalSound.h>
#include <util/CheckState.h>

namespace urchin {

    GlobalSound::GlobalSound(std::string filename, SoundCategory category, float initialVolume) :
            Sound(std::move(filename), category, initialVolume) {

    }

    void GlobalSound::initializeSource(ALuint sourceId) const {
        alSourcei(sourceId, AL_SOURCE_RELATIVE, true);
        CheckState::check("set source relative to true");

        alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f); //global sound: always position at 0 distance to listener
        CheckState::check("set source position to origin");
    }

    void GlobalSound::updateSource(ALuint) {
        //nothing to update
    }

    Sound::SoundType GlobalSound::getSoundType() const {
        return Sound::GLOBAL;
    }

}
