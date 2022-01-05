#pragma once

#include <trigger/SoundTrigger.h>
#include <trigger/shape/SoundShape.h>

namespace urchin {

    /**
    * Trigger performed when inside a defined zone
    */
    class ZoneTrigger : public SoundTrigger {
        public:
            ZoneTrigger(PlayBehavior, std::unique_ptr<const SoundShape>);

            const std::vector<SoundTrigger::TriggerAction>& evaluateTrigger(const Point3<float>&) override;

            const SoundShape& getSoundShape() const;
            void setSoundShape(std::unique_ptr<const SoundShape>);

        private:
            std::unique_ptr<const SoundShape> soundShape;
            bool isPlaying;
    };

}
