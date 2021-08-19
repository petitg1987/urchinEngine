#include <AL/alc.h>
#include <stdexcept>

#include <player/AudioPlayer.h>

namespace urchin {

    AudioPlayer::AudioPlayer(const Sound& sound) :
            sound(sound),
            sourceId(0),
            volumePercentageChange(1.0f) {
        if (!alcGetCurrentContext()) {
            throw std::runtime_error("No OpenAL context found: check that a sound manager has been created");
        }

        alGenSources(1, &sourceId);
        sound.initializeSource(sourceId);

        applyVolume();
    }

    AudioPlayer::~AudioPlayer() {
        alSourcei(sourceId, AL_BUFFER, 0);
        alDeleteSources(1, &sourceId);
    }

    bool AudioPlayer::isPlaying() const {
        ALint status;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &status);
        return status == AL_PLAYING;
    }

    bool AudioPlayer::isPaused() const {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        return state == AL_PAUSED;
    }

    /**
     * @return True if source is stopped (not playing, not in pause)
     */
    bool AudioPlayer::isStopped() const {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        return state == AL_STOPPED;
    }

    void AudioPlayer::changeVolume(float volumePercentageChange) {
        if (this->volumePercentageChange != volumePercentageChange) {
            this->volumePercentageChange = volumePercentageChange;
            applyVolume();
        }
    }

    void AudioPlayer::applyVolume() const {
        alSourcef(sourceId, AL_GAIN, sound.getInitialVolume() * volumePercentageChange);
    }

    const Sound& AudioPlayer::getSound() const {
        return sound;
    }

    ALuint AudioPlayer::getSourceId() const {
        return sourceId;
    }

}
