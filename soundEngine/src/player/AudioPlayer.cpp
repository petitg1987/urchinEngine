#include <AL/alc.h>
#include <stdexcept>

#include <player/AudioPlayer.h>
#include <util/CheckState.h>

namespace urchin {

    AudioPlayer::AudioPlayer(const Sound& sound) :
            sound(sound),
            sourceId(0),
            volumePercentageChange(1.0f) {
        if (!alcGetCurrentContext()) {
            throw std::runtime_error("No OpenAL context found: check that a sound manager has been created");
        }

        alGenSources(1, &sourceId);
        CheckState::check("generate source id");
        if (sourceId == 0) {
            Logger::instance().logError("Impossible to generate source id for sound: " + sound.getFilename());
        } else {
            sound.initializeSource(sourceId);
            applyVolume();
        }
    }

    AudioPlayer::~AudioPlayer() {
        if (sourceId != 0) {
            #ifdef URCHIN_DEBUG
                assert(getSourceState() == AL_INITIAL || getSourceState() == AL_STOPPED);
            #endif
            alSourcei(sourceId, AL_BUFFER, AL_NONE);
            CheckState::check("release source buffer (audio player)");

            alDeleteSources(1, &sourceId);
            CheckState::check("delete source");
        }
    }

    ALint AudioPlayer::getSourceState() const {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        CheckState::check("get source state");
        return state;
    }

    bool AudioPlayer::isPlaying() const {
        return getSourceState() == AL_PLAYING;
    }

    bool AudioPlayer::isPaused() const {
        return getSourceState() == AL_PAUSED;
    }

    /**
     * @return True if source is stopped (not playing, not in pause)
     */
    bool AudioPlayer::isStopped() const {
        return getSourceState() == AL_STOPPED;
    }

    void AudioPlayer::changeVolume(float volumePercentageChange) {
        if (this->volumePercentageChange != volumePercentageChange) {
            this->volumePercentageChange = volumePercentageChange;
            applyVolume();
        }
    }

    void AudioPlayer::applyVolume() const {
        float sourceVolume = sound.getInitialVolume() * volumePercentageChange;
        alSourcef(sourceId, AL_GAIN, sourceVolume);
        CheckState::check("set source gain", sourceVolume);
    }

    const Sound& AudioPlayer::getSound() const {
        return sound;
    }

    ALuint AudioPlayer::getSourceId() const {
        return sourceId;
    }

}
