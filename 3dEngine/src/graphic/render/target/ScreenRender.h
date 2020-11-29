#ifndef URCHINENGINE_SCREENRENDER_H
#define URCHINENGINE_SCREENRENDER_H

#include "graphic/render/target/RenderTarget.h"

namespace urchin {

    class ScreenRender : public RenderTarget {
        public:
            ~ScreenRender() override = default;

            void resetDisplay() const override;
            void display(const std::unique_ptr<GenericRenderer>&) const override;
    };

}

#endif
