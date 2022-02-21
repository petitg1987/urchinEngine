#pragma once

namespace urchin {

    class AbstractUIAnimation {
        public:
            AbstractUIAnimation();
            virtual ~AbstractUIAnimation() = default;

            void setupAnimationSpeed(float, std::unique_ptr<Bezier> = std::unique_ptr<Bezier>(nullptr));
            float getAnimationSpeed() const;

            void animate(float);
            virtual bool isCompleted() const = 0;

        protected:
            virtual void initializeAnimation();
            virtual void doAnimation(float) = 0;

            float computeProgression(float);

        private:
            bool animationInitialized;

            float animationSpeed;
            std::unique_ptr<Bezier> bezier;
    };

}