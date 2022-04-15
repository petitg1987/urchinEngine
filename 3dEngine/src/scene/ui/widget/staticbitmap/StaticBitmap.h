#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <resources/image/Image.h>
#include <api/render/GenericRenderer.h>

namespace urchin {

    class StaticBitmap : public Widget {
        public:
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, Size, const std::string&);
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, Size, const std::shared_ptr<Image>&);

            WidgetType getWidgetType() const override;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            StaticBitmap(Position, Size, std::shared_ptr<Texture>);

            //visual
            std::shared_ptr<Texture> texture;
            std::unique_ptr<GenericRenderer> bitmapRenderer;
    };

}
