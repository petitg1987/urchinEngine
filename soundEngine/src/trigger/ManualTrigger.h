#pragma once

#include <trigger/SoundTrigger.h>

namespace urchin {

    /**
    * Trigger performed by the user
    */
    class ManualTrigger : public SoundTrigger {
        public:
            explicit ManualTrigger(PlayBehavior);

            void playNew();
            void stopAll();
            void pauseAll();

            const std::vector<SoundTrigger::TriggerAction>& evaluateTrigger(const Point3<float>&) override;

        private:
            enum ManualTriggerAction {
                PLAY_NEW,
                PLAY_NEW_LOOP,
                STOP_ALL,
                PAUSE_ALL
            };

            std::vector<ManualTriggerAction> manualTriggerActions;
    };

}
