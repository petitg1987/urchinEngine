#include <trigger/SoundTrigger.h>

namespace urchin {

    SoundTrigger::SoundTrigger(TriggerType triggerType, PlayBehavior playBehavior) :
            triggerType(triggerType),
            playBehavior(playBehavior) {

    }

    SoundTrigger::TriggerType SoundTrigger::getTriggerType() const {
        return triggerType;
    }

    SoundTrigger::PlayBehavior SoundTrigger::getPlayBehavior() const {
        return playBehavior;
    }

}
