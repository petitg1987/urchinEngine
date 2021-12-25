#pragma once

namespace urchin {

    class CharacterEventCallback {
        public:
            CharacterEventCallback();
            virtual ~CharacterEventCallback() = default;

            void notifyWalking();
            void notifyRunning();
            void notifyNoStepping();

            virtual void onStartWalking();
            virtual void onStopWalking();
            virtual void onStartRunning();
            virtual void onStopRunning();
            virtual void onHitGround(float);
            virtual void onStartJumping();

        private:
            bool isWalking;
            bool isRunning;
    };

}
