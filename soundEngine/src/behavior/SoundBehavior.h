#ifndef URCHINENGINE_SOUNDBEHAVIOR_H
#define URCHINENGINE_SOUNDBEHAVIOR_H

namespace urchin {

    class SoundBehavior {
        public:
            enum PlayBehavior {
                PLAY_ONCE,
                PLAY_LOOP
            };

            enum StopBehavior {
                INSTANT_STOP,
                SMOOTH_STOP
            };

            SoundBehavior(PlayBehavior, StopBehavior);
            SoundBehavior(PlayBehavior, StopBehavior, float);

            PlayBehavior getPlayBehavior() const;
            StopBehavior getStopBehavior() const;

            float getVolumeDecreasePercentageOnStop() const;

        private:
            PlayBehavior playBehavior;
            StopBehavior stopBehavior;

            float volumeDecreasePercentageOnStop;
    };

}

#endif
