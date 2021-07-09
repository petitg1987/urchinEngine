#include <trigger/ShapeTrigger.h>

namespace urchin {

    /**
     * @param soundShape Delimited shape which trigger the sound play
     */
    ShapeTrigger::ShapeTrigger(PlayBehavior playBehavior, std::unique_ptr<const SoundShape> soundShape) :
            SoundTrigger(SoundTrigger::SHAPE_TRIGGER, playBehavior),
            soundShape(std::move(soundShape)),
            isPlaying(false) {

    }

    SoundTrigger::TriggerResultValue ShapeTrigger::evaluateTrigger(const Point3<float>& listenerPosition) {
        if (!isPlaying && soundShape->pointInsidePlayShape(listenerPosition)) {
            isPlaying = true;
            if (getPlayBehavior() == SoundTrigger::PLAY_LOOP) {
                return SoundTrigger::PLAYING_LOOP;
            }
            return SoundTrigger::PLAYING;
        }
        if (isPlaying && !soundShape->pointInsideStopShape(listenerPosition)) {
            isPlaying = false;
            return SoundTrigger::STOPPED;
        }

        return SoundTrigger::NO_TRIGGER;
    }

    const SoundShape& ShapeTrigger::getSoundShape() const {
        return *soundShape;
    }

}
