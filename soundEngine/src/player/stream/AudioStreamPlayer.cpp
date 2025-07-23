#include <AL/al.h>

#include "player/stream/AudioStreamPlayer.h"
#include "player/stream/StreamUpdateWorker.h"
#include "util/CheckState.h"

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
        CheckState::check("source pause");
    }

    void AudioStreamPlayer::unpause() {
        alSourcePlay(getSourceId());
        CheckState::check("source unpause");
    }

    void AudioStreamPlayer::stop() {
        alSourceStop(getSourceId());
        CheckState::check("source stop");

        streamUpdateWorker.removeTask(*this);
    }

    void AudioStreamPlayer::play(bool playLoop) const {
        streamUpdateWorker.addTask(*this, playLoop);

        alSourcePlay(getSourceId());
        CheckState::check("source play");
    }

}
