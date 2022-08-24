#pragma once

#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    class UIAnimationSequencer final : public AbstractUIAnimation {
        public:
            explicit UIAnimationSequencer(std::vector<std::unique_ptr<AbstractUIAnimation>>);

            unsigned int getAnimationIndex() const;

        protected:
            void doAnimation(float) override;

        private:
            unsigned int currentAnimationIndex;
            std::vector<std::unique_ptr<AbstractUIAnimation>> animations;
    };

}