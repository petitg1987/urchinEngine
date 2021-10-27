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
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, Size, std::shared_ptr<Image>);

            const std::string& getImageName() const;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&, const Matrix3<float>&) override;

        private:
            StaticBitmap(Position, Size, std::shared_ptr<Image>);

            //properties
            std::shared_ptr<Image> image;

            //visual
            std::shared_ptr<Texture> tex;
            std::unique_ptr<GenericRenderer> bitmapRenderer;
    };

}
