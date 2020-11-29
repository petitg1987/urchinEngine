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
            StaticBitmap(Position, Size, std::string);
            ~StaticBitmap() override = default;

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
