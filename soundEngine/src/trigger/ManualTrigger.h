#pragma once

#include <trigger/SoundTrigger.h>

namespace urchin {

    /**
    * Trigger activated by the user
    */
    class ManualTrigger final : public SoundTrigger {
        public:
            explicit ManualTrigger(PlayBehavior);

            void playNew();
            void stopAll();
            void pauseAll();
            void unpauseAll();

            unsigned int countSoundToPlay() const;

            const std::vector<TriggerAction>& evaluateTrigger(const Point3<float>&) override;

        private:
            enum ManualTriggerAction {
                PLAY_NEW,
                PLAY_NEW_LOOP,
                STOP_ALL,
                PAUSE_ALL,
                UNPAUSE_ALL
            };

            std::vector<ManualTriggerAction> manualTriggerActions;
    };

}
