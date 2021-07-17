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

    class Window : public Widget {
        public:
            static std::shared_ptr<Window> newWindow(Widget*, Position, Size, std::string, std::string);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            Window(Position, Size, std::string, std::string);

            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            //properties
            const std::string skinName;
            const std::string titleKey;

            //state
            int mousePositionX, mousePositionY;
            enum windowStates {
                DEFAULT,
                MOVING,
                CLOSING
            } state;

            //visual
            std::shared_ptr<Text> title;
            std::shared_ptr<Texture> texWindow;
            std::unique_ptr<GenericRenderer> windowRenderer;
    };

}
