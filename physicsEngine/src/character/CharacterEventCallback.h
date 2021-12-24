#pragma once

namespace urchin {

    class CharacterEventCallback {
        public:
            virtual void startMoving() = 0;
            virtual void stopMoving() = 0;

            virtual void startRun() = 0;
            virtual void stopRun() = 0;

            virtual void fall(float) = 0;
            virtual void jump() = 0;
    };

}
