#pragma once

namespace urchin {

    class SoundBehavior { //TODO simplify (remove class, remove soundBehavior tag in mapEditor + visual)
        public:
            enum PlayBehavior {
                PLAY_ONCE,
                PLAY_LOOP
            };

            explicit SoundBehavior(PlayBehavior);

            PlayBehavior getPlayBehavior() const;

        private:
            PlayBehavior playBehavior;
    };

}
