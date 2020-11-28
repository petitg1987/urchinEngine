#ifndef URCHINENGINE_RENDERTARGET_H
#define URCHINENGINE_RENDERTARGET_H

#include <memory>

#include "graphic/render/GenericRenderer.h"
#include "graphic/shader/model/Shader.h"

namespace urchin {

    class RenderTarget {
        public:
            RenderTarget();
            virtual ~RenderTarget() = default;

            void onResize(unsigned int, unsigned int);

            void activeShader(const std::unique_ptr<Shader> &) const;
            virtual void resetDisplay() const = 0;
            virtual void display(const std::unique_ptr<GenericRenderer> &) const = 0;

        protected:
            void executeRenderer(const std::unique_ptr<GenericRenderer> &) const;

            unsigned int getTargetWidth() const;
            unsigned int getTargetHeight() const;

        private:
            unsigned int targetWidth, targetHeight;
    };

}

#endif
