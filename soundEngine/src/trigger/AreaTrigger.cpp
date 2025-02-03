#include <trigger/AreaTrigger.h>

namespace urchin {

    /**
     * @param soundShape Delimited shape which trigger the sound play
     */
    AreaTrigger::AreaTrigger(PlayBehavior playBehavior, std::unique_ptr<SoundShape> soundShape) :
            SoundTrigger(AREA_TRIGGER, playBehavior),
            soundShape(std::move(soundShape)),
            isPlaying(false) {

    }

    const std::vector<SoundTrigger::TriggerAction>& AreaTrigger::evaluateTrigger(const Point3<float>& listenerPosition) {
        triggerActions.clear();

        if (!isPlaying && soundShape->pointInsidePlayShape(listenerPosition)) {
            isPlaying = true;
            if (getPlayBehavior() == PlayBehavior::PLAY_LOOP) {
                triggerActions.emplace_back(PLAY_NEW_LOOP);
            } else if (getPlayBehavior() == PlayBehavior::PLAY_ONCE) {
                triggerActions.emplace_back(PLAY_NEW);
            } else {
                throw std::runtime_error("Unknown play behavior: " + std::to_string((int)getPlayBehavior()));
            }
        } else if (isPlaying && !soundShape->pointInsideStopShape(listenerPosition)) {
            isPlaying = false;
            triggerActions.emplace_back(STOP_ALL);
        }

        return triggerActions;
    }

    SoundShape& AreaTrigger::getSoundShape() {
        return *soundShape;
    }

    const SoundShape& AreaTrigger::getSoundShape() const {
        return *soundShape;
    }

    void AreaTrigger::setSoundShape(std::unique_ptr<SoundShape> soundShape) {
        this->soundShape = std::move(soundShape);
    }

    std::unique_ptr<SoundTrigger> AreaTrigger::clone(std::shared_ptr<Sound>) const {
        return std::make_unique<AreaTrigger>(getPlayBehavior(), soundShape->clone());
    }

}
