#pragma once

#include <trigger/SoundTrigger.h>
#include <trigger/shape/SoundShape.h>

namespace urchin {

    /**
    * Trigger performed when inside a defined zone
    */
    class ZoneTrigger final : public SoundTrigger {
        public:
            ZoneTrigger(PlayBehavior, std::unique_ptr<SoundShape>);

            const std::vector<SoundTrigger::TriggerAction>& evaluateTrigger(const Point3<float>&) override;

            SoundShape& getSoundShape();
            const SoundShape& getSoundShape() const;
            void setSoundShape(std::unique_ptr<SoundShape>);

        private:
            std::unique_ptr<SoundShape> soundShape;
            bool isPlaying;
    };

}
