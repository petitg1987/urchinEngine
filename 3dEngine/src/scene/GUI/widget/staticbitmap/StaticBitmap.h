#ifndef URCHINENGINE_STATICBITMAP_H
#define URCHINENGINE_STATICBITMAP_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "resources/image/Image.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class StaticBitmap : public Widget {
        public:
            StaticBitmap(Position, Size, std::string );
            ~StaticBitmap() override;

            void createOrUpdateWidget() override;

            void display(const std::unique_ptr<Shader> &, const ShaderVar &, float) override;

        private:
            //properties
            const std::string filename;

            //visual
            Image *tex;
            std::unique_ptr<GenericRenderer> bitmapRenderer;
    };

}

#endif
