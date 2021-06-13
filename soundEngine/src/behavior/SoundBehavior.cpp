#include <behavior/SoundBehavior.h>

namespace urchin {

    SoundBehavior::SoundBehavior(PlayBehavior playBehavior) :
        playBehavior(playBehavior) {

    }

    SoundBehavior::PlayBehavior SoundBehavior::getPlayBehavior() const {
        return playBehavior;
    }

}
