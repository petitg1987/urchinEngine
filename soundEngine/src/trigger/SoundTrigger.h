#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class SoundTrigger {
        public:
            enum TriggerType {
                MANUAL_TRIGGER,
                SHAPE_TRIGGER
            };

            enum PlayBehavior {
                PLAY_ONCE,
                PLAY_LOOP
            };

            enum TriggerResultValue {
                NO_TRIGGER,
                PLAYING,
                PLAYING_LOOP,
                STOPPED,
                PAUSED
            };

            SoundTrigger(TriggerType, PlayBehavior);
            virtual ~SoundTrigger() = default;

            TriggerType getTriggerType() const;
            PlayBehavior getPlayBehavior() const;

            virtual TriggerResultValue evaluateTrigger(const Point3<float>&) = 0;

        private:
            TriggerType triggerType;
            PlayBehavior playBehavior;
    };

}
