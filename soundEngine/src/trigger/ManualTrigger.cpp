#include <stdexcept>

#include <trigger/ManualTrigger.h>

namespace urchin {

    ManualTrigger::ManualTrigger(PlayBehavior playBehavior) :
            SoundTrigger(SoundTrigger::MANUAL_TRIGGER, playBehavior) {

    }

    void ManualTrigger::playNew() {
        if (getPlayBehavior() == PlayBehavior::PLAY_LOOP) {
            manualTriggerActions.emplace_back(ManualTriggerAction::PLAY_NEW_LOOP);
        } else {
            manualTriggerActions.emplace_back(ManualTriggerAction::PLAY_NEW);
        }
    }

    void ManualTrigger::stopAll() {
        manualTriggerActions.emplace_back(ManualTriggerAction::STOP_ALL);
    }

    void ManualTrigger::pauseAll() {
        manualTriggerActions.emplace_back(ManualTriggerAction::PAUSE_ALL);
    }

    void ManualTrigger::unpauseAll() {
        manualTriggerActions.emplace_back(ManualTriggerAction::UNPAUSE_ALL);
    }

    unsigned int ManualTrigger::countSoundToPlay() const {
        unsigned int count = 0;
        for (auto manualTriggerAction : manualTriggerActions) {
            if (manualTriggerAction == ManualTriggerAction::PLAY_NEW || manualTriggerAction == ManualTriggerAction::PLAY_NEW_LOOP) {
                count++;
            }
        }
        return count;
    }

    const std::vector<SoundTrigger::TriggerAction>& ManualTrigger::evaluateTrigger(const Point3<float>&) {
        triggerActions.clear();

        for (auto manualTriggerAction : manualTriggerActions) {
            if (manualTriggerAction == ManualTriggerAction::PLAY_NEW) {
                triggerActions.emplace_back(SoundTrigger::PLAY_NEW);
            } else if (manualTriggerAction == ManualTriggerAction::PLAY_NEW_LOOP) {
                triggerActions.emplace_back(SoundTrigger::PLAY_NEW_LOOP);
            } else if (manualTriggerAction == ManualTriggerAction::STOP_ALL) {
                triggerActions.emplace_back(SoundTrigger::STOP_ALL);
            } else if (manualTriggerAction == ManualTriggerAction::PAUSE_ALL) {
                triggerActions.emplace_back(SoundTrigger::PAUSE_ALL);
            } else if (manualTriggerAction == ManualTriggerAction::UNPAUSE_ALL) {
                triggerActions.emplace_back(SoundTrigger::UNPAUSE_ALL);
            } else {
                throw std::invalid_argument("Unknown manual trigger action: " + std::to_string(manualTriggerAction));
            }
        }

        manualTriggerActions.clear();
        return triggerActions;
    }

}
