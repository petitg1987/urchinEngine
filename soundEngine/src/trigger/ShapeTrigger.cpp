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

    const std::vector<SoundTrigger::TriggerAction>& ShapeTrigger::evaluateTrigger(const Point3<float>& listenerPosition) {
        triggerActions.clear();

        if (!isPlaying && soundShape->pointInsidePlayShape(listenerPosition)) {
            isPlaying = true;
            if (getPlayBehavior() == SoundTrigger::PLAY_LOOP) {
                triggerActions.emplace_back(SoundTrigger::PLAY_NEW_LOOP);
            } else if (getPlayBehavior() == SoundTrigger::PLAY_ONCE) {
                triggerActions.emplace_back(SoundTrigger::PLAY_NEW);
            } else {
                throw std::runtime_error("Unknown play behavior: " + std::to_string(getPlayBehavior()));
            }
        }else if (isPlaying && !soundShape->pointInsideStopShape(listenerPosition)) {
            isPlaying = false;
            triggerActions.emplace_back(SoundTrigger::STOP_ALL);
        }

        return triggerActions;
    }

    const SoundShape& ShapeTrigger::getSoundShape() const {
        return *soundShape;
    }

}
