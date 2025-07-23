#pragma once

#include "trigger/SoundTrigger.h"
#include "sound/Sound.h"

namespace urchin {

    /**
    * Trigger activated when the sound is audible
    */
    class AutoTrigger final : public SoundTrigger {
        public:
            AutoTrigger(PlayBehavior, const std::shared_ptr<Sound>&);

            const std::vector<TriggerAction>& evaluateTrigger(const Point3<float>&) override;

            std::unique_ptr<SoundTrigger> clone(const std::shared_ptr<Sound>&) const override;

        private:
            std::weak_ptr<Sound> sound;
            bool isPlaying;
    };

}