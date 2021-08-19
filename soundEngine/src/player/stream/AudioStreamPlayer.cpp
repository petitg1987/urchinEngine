#include <AL/al.h>

#include <player/stream/AudioStreamPlayer.h>
#include <player/stream/StreamUpdateWorker.h>

namespace urchin {

    AudioStreamPlayer::AudioStreamPlayer(const Sound& sound, StreamUpdateWorker& streamUpdateWorker) :
            AudioPlayer(sound),
            streamUpdateWorker(streamUpdateWorker) {

    }

    AudioStreamPlayer::~AudioStreamPlayer() {
        streamUpdateWorker.removeTask(*this);
    }

    void AudioStreamPlayer::play() {
        play(false);
    }

    void AudioStreamPlayer::playLoop() {
        play(true);
    }

    void AudioStreamPlayer::pause() {
        alSourcePause(getSourceId());
    }

    void AudioStreamPlayer::unpause() {
        alSourcePlay(getSourceId());
    }

    void AudioStreamPlayer::stop() {
        alSourceStop(getSourceId());
        streamUpdateWorker.removeTask(*this);
    }

    void AudioStreamPlayer::play(bool playLoop) {
        streamUpdateWorker.addTask(*this, playLoop);
        alSourcePlay(getSourceId());
    }

}
