#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/widget/text/Text.h>
#include <resources/image/Image.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class Window : public Widget {
        public:
            static std::shared_ptr<Window> create(Widget*, Position, Size, std::string, std::string);

            WidgetType getWidgetType() const override;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Window(Position, Size, std::string, std::string);

            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;
            void onResetStateEvent() override;

            //properties
            const std::string skinName;
            const std::string titleKey;

            //state
            int mousePositionX;
            int mousePositionY;
            enum WindowStates {
                DEFAULT,
                MOVING,
                CLOSING
            };
            WindowStates state;

            //visual
            std::shared_ptr<Text> title;
            std::shared_ptr<Texture> texWindow;
            std::unique_ptr<GenericRenderer> windowRenderer;
    };

}
