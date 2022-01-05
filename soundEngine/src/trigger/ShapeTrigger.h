#pragma once

#include <trigger/SoundTrigger.h>
#include <trigger/shape/SoundShape.h>

namespace urchin {

    /**
    * Trigger performed when inside a shape
    */
    class ShapeTrigger : public SoundTrigger { //TODO rename in ZoneTrigger ?
        public:
            ShapeTrigger(PlayBehavior, std::unique_ptr<const SoundShape>);

            const std::vector<SoundTrigger::TriggerAction>& evaluateTrigger(const Point3<float>&) override;

            const SoundShape& getSoundShape() const;

        private:
            std::unique_ptr<const SoundShape> soundShape;
            bool isPlaying;
    };

}
