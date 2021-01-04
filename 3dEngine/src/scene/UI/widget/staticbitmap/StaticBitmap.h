#ifndef URCHINENGINE_STATICBITMAP_H
#define URCHINENGINE_STATICBITMAP_H

#include <string>
#include <memory>

#include "scene/UI/widget/Widget.h"
#include "scene/UI/widget/Position.h"
#include "scene/UI/widget/Size.h"
#include "resources/image/Image.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class StaticBitmap : public Widget {
        public:
            StaticBitmap(Widget*, Position, Size, std::string);
            StaticBitmap(Position, Size, std::string);

            void createOrUpdateWidget() override;

            void display(const RenderTarget*, const ShaderVar&, float) override;

        private:
            //properties
            const std::string filename;

            //visual
            std::shared_ptr<Texture> tex;
            std::unique_ptr<GenericRenderer> bitmapRenderer;
    };

}

#endif
