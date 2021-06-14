#pragma once

#include <trigger/SoundTrigger.h>

namespace urchin {

    /**
    * Trigger performed by the user
    */
    class ManualTrigger : public SoundTrigger {
        public:
            explicit ManualTrigger(PlayBehavior);

            void play();
            void stop();
            void pause();

            SoundTrigger::TriggerResultValue evaluateTrigger(const Point3<float>&) override;

        private:
            enum ManualTriggerValue {
                NO_TRIGGER,

                PLAY,
                PLAY_LOOP,
                STOP,
                PAUSE
            };

            ManualTriggerValue manualTriggerValue;
    };

}
