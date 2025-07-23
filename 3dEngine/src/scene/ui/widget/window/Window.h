#pragma once

#include <string>
#include <memory>

#include "scene/ui/widget/Widget.h"
#include "scene/ui/widget/Position.h"
#include "scene/ui/widget/Size.h"
#include "scene/ui/widget/text/Text.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class Window final : public Widget {
        public:
            static std::shared_ptr<Window> create(Widget*, Position, Size, std::string, std::string);

            WidgetType getWidgetType() const override;

        private:
            Window(Position, Size, std::string, std::string);

            void createOrUpdateWidget() override;
            bool requireRenderer() const override;

            bool onKeyPressEvent(InputDeviceKey) override;
            bool onKeyReleaseEvent(InputDeviceKey) override;
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
    };

}
