#pragma once

namespace urchin {

    class AbstractUIAnimation {
        public:
            AbstractUIAnimation();
            virtual ~AbstractUIAnimation() = default;

            void animate(float);
            virtual bool isCompleted() const = 0;

        protected:
            virtual void initializeAnimation();
            virtual void doAnimation(float) = 0;

        private:
            bool animationInitialized;
    };

}