#pragma once

#include <scene/ui/animation/AbstractUIAnimation.h>

namespace urchin {

    class UIAnimationParallelizer : public AbstractUIAnimation {
        public:
            explicit UIAnimationParallelizer(std::vector<std::unique_ptr<AbstractUIAnimation>>);

            void doAnimation(float) override;
            bool isCompleted() const override;

        private:
            std::vector<std::unique_ptr<AbstractUIAnimation>> animations;
    };

}