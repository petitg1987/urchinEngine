#pragma once

#include "trigger/SoundTrigger.h"
#include "trigger/shape/SoundShape.h"

namespace urchin {

    /**
    * Trigger activated when the listener is inside a defined area
    */
    class AreaTrigger final : public SoundTrigger {
        public:
            AreaTrigger(PlayBehavior, std::unique_ptr<SoundShape>);

            const std::vector<TriggerAction>& evaluateTrigger(const Point3<float>&) override;

            SoundShape& getSoundShape();
            const SoundShape& getSoundShape() const;
            void setSoundShape(std::unique_ptr<SoundShape>);

            std::unique_ptr<SoundTrigger> clone(const std::shared_ptr<Sound>&) const override;

        private:
            std::unique_ptr<SoundShape> soundShape;
            bool isPlaying;
    };

}
