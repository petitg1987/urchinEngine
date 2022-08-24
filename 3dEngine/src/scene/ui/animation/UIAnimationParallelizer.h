#pragma once

#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    class UIAnimationParallelizer final : public AbstractUIAnimation {
        public:
            explicit UIAnimationParallelizer(std::vector<std::unique_ptr<AbstractUIAnimation>>);

        protected:
            void doAnimation(float) override;

        private:
            std::vector<std::unique_ptr<AbstractUIAnimation>> animations;
    };

}