#pragma once

namespace urchin {

    class AbstractUIAnimation {
        public:
            AbstractUIAnimation();
            virtual ~AbstractUIAnimation() = default;

            void setupAnimationSpeed(float, std::unique_ptr<Bezier> = std::unique_ptr<Bezier>(nullptr));
            float getAnimationSpeed() const;

            void animate(float);
            bool isCompleted() const;

        protected:
            virtual void initializeAnimation();
            virtual void doAnimation(float) = 0;

            void markCompleted();
            float computeProgression(float) const;

        private:
            bool animationInitialized;
            bool animationCompleted;

            float animationSpeed;
            std::unique_ptr<Bezier> bezier;
    };

}