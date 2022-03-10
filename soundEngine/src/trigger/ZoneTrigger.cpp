#include <trigger/ZoneTrigger.h>

namespace urchin {

    /**
     * @param soundShape Delimited shape which trigger the sound play
     */
    ZoneTrigger::ZoneTrigger(PlayBehavior playBehavior, std::unique_ptr<SoundShape> soundShape) :
            SoundTrigger(SoundTrigger::ZONE_TRIGGER, playBehavior),
            soundShape(std::move(soundShape)),
            isPlaying(false) {

    }

    const std::vector<SoundTrigger::TriggerAction>& ZoneTrigger::evaluateTrigger(const Point3<float>& listenerPosition) {
        triggerActions.clear();

        if (!isPlaying && soundShape->pointInsidePlayShape(listenerPosition)) {
            isPlaying = true;
            if (getPlayBehavior() == PlayBehavior::PLAY_LOOP) {
                triggerActions.emplace_back(SoundTrigger::PLAY_NEW_LOOP);
            } else if (getPlayBehavior() == PlayBehavior::PLAY_ONCE) {
                triggerActions.emplace_back(SoundTrigger::PLAY_NEW);
            } else {
                throw std::runtime_error("Unknown play behavior: " + std::to_string((int)getPlayBehavior()));
            }
        }else if (isPlaying && !soundShape->pointInsideStopShape(listenerPosition)) {
            isPlaying = false;
            triggerActions.emplace_back(SoundTrigger::STOP_ALL);
        }

        return triggerActions;
    }

    SoundShape& ZoneTrigger::getSoundShape() {
        return *soundShape;
    }

    const SoundShape& ZoneTrigger::getSoundShape() const {
        return *soundShape;
    }

    void ZoneTrigger::setSoundShape(std::unique_ptr<SoundShape> soundShape) {
        this->soundShape = std::move(soundShape);
    }

}
