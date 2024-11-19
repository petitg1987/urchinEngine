#pragma once

#include <UrchinCommon.h>

#include <trigger/PlayBehavior.h>

namespace urchin {

    class SoundTrigger {
        public:
            enum TriggerType {
                AUTO_TRIGGER,
                MANUAL_TRIGGER,
                AREA_TRIGGER
            };

            enum TriggerAction {
                PLAY_NEW,
                PLAY_NEW_LOOP,
                STOP_ALL,
                PAUSE_ALL,
                UNPAUSE_ALL
            };

            SoundTrigger(TriggerType, PlayBehavior);
            virtual ~SoundTrigger() = default;

            TriggerType getTriggerType() const;

            PlayBehavior getPlayBehavior() const;
            void setPlayBehavior(PlayBehavior);

            virtual const std::vector<TriggerAction>& evaluateTrigger(const Point3<float>&) = 0;

        protected:
            std::vector<TriggerAction> triggerActions;

        private:
            TriggerType triggerType;
            PlayBehavior playBehavior;
    };

}
