#include <stdexcept>

#include <trigger/ManualTrigger.h>

namespace urchin {

    ManualTrigger::ManualTrigger(PlayBehavior playBehavior) :
            SoundTrigger(SoundTrigger::MANUAL_TRIGGER, playBehavior),
            manualTriggerValue(ManualTriggerValue::STOP) {

    }

    void ManualTrigger::play() {
        if (getPlayBehavior() == SoundTrigger::PLAY_LOOP) {
            manualTriggerValue = ManualTriggerValue::PLAY_LOOP;
        } else {
            manualTriggerValue = ManualTriggerValue::PLAY;
        }
    }

    void ManualTrigger::stop() {
        manualTriggerValue = ManualTriggerValue::STOP;
    }

    void ManualTrigger::pause() {
        manualTriggerValue = ManualTriggerValue::PAUSE;
    }

    SoundTrigger::TriggerResultValue ManualTrigger::evaluateTrigger(const Point3<float>&) {
        SoundTrigger::TriggerResultValue result;

        if (manualTriggerValue == ManualTriggerValue::PLAY) {
            result = SoundTrigger::PLAYING;
        } else if (manualTriggerValue == ManualTriggerValue::PLAY_LOOP) {
            result = SoundTrigger::PLAYING_LOOP;
        } else if (manualTriggerValue == ManualTriggerValue::STOP) {
            result = SoundTrigger::STOPPED;
        } else if (manualTriggerValue == ManualTriggerValue::PAUSE) {
            result = SoundTrigger::PAUSED;
        } else if (manualTriggerValue == ManualTriggerValue::NO_TRIGGER) {
            result = SoundTrigger::NO_TRIGGER;
        } else {
            throw std::invalid_argument("Unknown manual trigger value: " + std::to_string(manualTriggerValue));
        }

        manualTriggerValue = NO_TRIGGER;
        return result;
    }

}
