#ifndef URCHINENGINE_RENDERTARGET_H
#define URCHINENGINE_RENDERTARGET_H

#include <memory>
#include <vector>

#include "graphic/shader/model/Shader.h"

namespace urchin {

    class GenericRenderer;

    class RenderTarget {
        public:
            RenderTarget();
            virtual ~RenderTarget() = default;

            void onResize(unsigned int, unsigned int);

            void addRenderer(GenericRenderer*);

            virtual void resetDisplay() const = 0;
            virtual void display(const std::unique_ptr<GenericRenderer>&) const = 0;

        protected:
            void executeRenderer(const std::unique_ptr<GenericRenderer>&) const;

            unsigned int getTargetWidth() const;
            unsigned int getTargetHeight() const;

        private:
            unsigned int targetWidth, targetHeight;

            std::vector<GenericRenderer*> renderers;
    };

}

#endif
