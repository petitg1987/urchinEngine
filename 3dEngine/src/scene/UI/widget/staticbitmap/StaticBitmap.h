#pragma once

#include <string>
#include <memory>

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Size.h>
#include <resources/image/Image.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class StaticBitmap : public Widget {
        public:
            StaticBitmap(Widget*, Position, Size, std::string);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            //properties
            const std::string filename;

            //visual
            std::shared_ptr<Texture> tex;
            std::shared_ptr<GenericRenderer> bitmapRenderer;
    };

}
