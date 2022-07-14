#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/staticbitmap/WidthSize.h>
#include <scene/ui/widget/staticbitmap/HeightSize.h>
#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <resources/image/Image.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class StaticBitmap : public Widget {
        public:
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, Size, std::shared_ptr<Texture>);
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, Size, const std::string&);
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, WidthSize, const std::string&);
            static std::shared_ptr<StaticBitmap> create(Widget*, Position, HeightSize, const std::string&);

            WidgetType getWidgetType() const override;

            void updateTexture(const std::string&);
            const std::string& getTextureName() const;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            StaticBitmap(Position, Size, std::shared_ptr<Texture>);

            static std::shared_ptr<Texture> buildTexture(const std::string&);

            //visual
            std::shared_ptr<Texture> texture;
    };

}
