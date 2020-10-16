#include "behavior/SoundBehavior.h"

#define DEFAULT_VOLUME_DECREASE_PERCENTAGE_ON_STOP 0.5

namespace urchin {

    /**
     * @param playBehavior Behavior of sound during playing
     * @param stopBehavior Behavior of sound during stopping
     */
    SoundBehavior::SoundBehavior(PlayBehavior playBehavior, StopBehavior stopBehavior) :
        playBehavior(playBehavior),
        stopBehavior(stopBehavior),
        volumeDecreasePercentageOnStop(DEFAULT_VOLUME_DECREASE_PERCENTAGE_ON_STOP) {

    }

    /**
     * Constructor
     * @param playBehavior Behavior of sound during playing
     * @param stopBehavior Behavior of sound during stopping
     * @param volumeDecreasePercentageOnStop Define the volume decrease percentage (value from 0.0 to 1.0) when sound is configured in smooth stop.
     * This percentage of decrease is applied each second.
     */
    SoundBehavior::SoundBehavior(PlayBehavior playBehavior, StopBehavior stopBehavior, float volumeDecreasePercentageOnStop) :
        playBehavior(playBehavior),
        stopBehavior(stopBehavior),
        volumeDecreasePercentageOnStop(volumeDecreasePercentageOnStop) {

    }

    SoundBehavior::PlayBehavior SoundBehavior::getPlayBehavior() const {
        return playBehavior;
    }

    SoundBehavior::StopBehavior SoundBehavior::getStopBehavior() const {
        return stopBehavior;
    }

    /**
     * @param Volume decrease percentage (value from 0.0 to 1.0) on stop
     */
    float SoundBehavior::getVolumeDecreasePercentageOnStop() const {
        return volumeDecreasePercentageOnStop;
    }

}
