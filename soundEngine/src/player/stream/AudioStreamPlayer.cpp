#include <AL/al.h>

#include "player/stream/AudioStreamPlayer.h"

namespace urchin {

    AudioStreamPlayer::AudioStreamPlayer(const Sound* sound, StreamUpdateWorker* streamUpdateWorker) :
        AudioPlayer(sound),
        streamUpdateWorker(streamUpdateWorker) {

    }

    void AudioStreamPlayer::play() {
        play(false);
    }

    void AudioStreamPlayer::playLoop() {
        play(true);
    }

    bool AudioStreamPlayer::isPlaying() {
        return streamUpdateWorker->isTaskExist(getSound());
    }

    void AudioStreamPlayer::pause() {
        alSourcePause(getSound()->getSourceId());
    }

    bool AudioStreamPlayer::isPaused() {
        ALint state;
        alGetSourcei(getSound()->getSourceId(), AL_SOURCE_STATE, &state);
        return state == AL_PAUSED;
    }

    void AudioStreamPlayer::stop() {
        alSourceStop(getSound()->getSourceId());

        streamUpdateWorker->removeTask(getSound());
    }

    void AudioStreamPlayer::play(bool playLoop) {
        if (!streamUpdateWorker->isTaskExist(getSound())) {
            streamUpdateWorker->addTask(getSound(), playLoop);
        }

        alSourcePlay(getSound()->getSourceId());
    }

}
