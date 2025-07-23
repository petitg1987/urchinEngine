#pragma once

#include <string>
#include <memory>

#include "scene/ui/widget/Widget.h"
#include "scene/ui/widget/Position.h"
#include "scene/ui/widget/Size.h"
#include "scene/ui/widget/text/Text.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class Button final : public Widget {
        public:
            static std::shared_ptr<Button> create(Widget*, Position, Size, std::string, std::string);

            WidgetType getWidgetType() const override;

        private:
            Button(Position, Size, std::string, std::string);

            void createOrUpdateWidget() override;
            bool requireRenderer() const override;
            void prepareWidgetRendering(float) override;

            void refreshTexture();

            bool onKeyPressEvent(InputDeviceKey) override;
            bool onKeyReleaseEvent(InputDeviceKey) override;
            bool onMouseMoveEvent(int, int) override;

            //properties
            const std::string skinName;

            //display information
            std::shared_ptr<Text> text;
            const std::string buttonText;

            //visual
            std::shared_ptr<Texture> texInfoDefault;
            std::shared_ptr<Texture> texInfoOnFocus;
            std::shared_ptr<Texture> texInfoOnClick;
    };

}
