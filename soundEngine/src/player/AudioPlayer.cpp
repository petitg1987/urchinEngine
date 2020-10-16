#include "player/AudioPlayer.h"

namespace urchin {

    AudioPlayer::AudioPlayer(const Sound *sound) :
        sound(sound) {

    }

    const Sound *AudioPlayer::getSound() const {
        return sound;
    }

}
