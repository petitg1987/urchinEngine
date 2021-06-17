#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/widget/text/Text.h>
#include <resources/image/Image.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class Button : public Widget {
        public:
            Button(Widget*, Position, Size, std::string, std::string);
            ~Button() override = default;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            void refreshTexture();

            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            //properties
            const std::string nameSkin;

            //display information
            Text* text;
            const std::string buttonText;

            //visual
            std::shared_ptr<Texture> currentTexture, texInfoDefault, texInfoOnFocus, texInfoOnClick;
            std::shared_ptr<GenericRenderer> buttonRenderer;
    };

}
