#include <utility>
#include <stdexcept>
#include <AL/alc.h>

#include <sound/Sound.h>

namespace urchin {

    Sound::Sound(std::string filename, SoundCategory category) :
            sourceId(0),
            filename(std::move(filename)),
            category(category),
            initialVolume(1.0f),
            volumePercentageChange(1.0f) {
        if (!alcGetCurrentContext()) {
            throw std::runtime_error("No OpenAL context found: check that a sound manager has been created");
        }

        alGenSources(1, &sourceId);
        applyVolume();
    }

    Sound::~Sound() {
        alSourcei(sourceId, AL_BUFFER, 0);
        alDeleteSources(1, &sourceId);
    }

    /**
     * @return Source id usable by OpenAL
     */
    ALuint Sound::getSourceId() const {
        return sourceId;
    }

    const std::string& Sound::getFilename() const {
        return filename;
    }

    Sound::SoundCategory Sound::getSoundCategory() const {
        return category;
    }

    /**
     * @return True if source is stopped (not playing, not in pause)
     */
    bool Sound::isStopped() const {
        ALint status;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &status);
        return status == AL_STOPPED;
    }

    bool Sound::isPaused() const {
        ALint status;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &status);
        return status == AL_PAUSED;
    }

    bool Sound::isPlaying() const {
        ALint status;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &status);
        return status == AL_PLAYING;
    }

    /**
     * @param initialVolume Set the initial sound volume (0.0=minimum volume, 1.0=original volume). Volume can be higher to 1.0.
     */
    void Sound::setInitialVolume(float initialVolume) {
        this->initialVolume = initialVolume;
        applyVolume();
    }

    float Sound::getInitialVolume() const {
        return initialVolume;
    }

    void Sound::changeVolume(float volumePercentageChange) {
        this->volumePercentageChange = volumePercentageChange;
        applyVolume();
    }

    void Sound::applyVolume() const {
        alSourcef(sourceId, AL_GAIN, initialVolume * volumePercentageChange);
    }

}
