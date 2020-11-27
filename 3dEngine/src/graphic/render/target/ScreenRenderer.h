#ifndef URCHINENGINE_SCREENRENDERER_H
#define URCHINENGINE_SCREENRENDERER_H

#include "graphic/render/target/TargetRenderer.h"

namespace urchin {

    class ScreenRenderer : public TargetRenderer {
        public:
            ~ScreenRenderer() override = default;

            void resetDraw() const override;
            void draw(const std::unique_ptr<GenericRenderer> &) const override;
    };

}

#endif
