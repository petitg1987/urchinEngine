#ifndef URCHINENGINE_ITARGETRENDERER_H
#define URCHINENGINE_ITARGETRENDERER_H

#include <memory>

#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class TargetRenderer {
        public:
            TargetRenderer();
            virtual ~TargetRenderer() = default;

            void onResize(unsigned int, unsigned int);

            virtual void resetDraw() const = 0;
            virtual void draw(const std::unique_ptr<GenericRenderer> &) const = 0;

        protected:
            unsigned int getTargetWidth() const;
            unsigned int getTargetHeight() const;

        private:
            unsigned int targetWidth, targetHeight;
    };

}

#endif
