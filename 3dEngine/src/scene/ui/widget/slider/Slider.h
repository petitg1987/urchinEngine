#pragma once

#include <string>
#include <vector>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <scene/ui/EventListener.h>

namespace urchin {

    class Slider final : public Widget {
        public:
            static std::shared_ptr<Slider> create(Widget*, Position, Size, std::string, const std::vector<std::string>&);
            static std::shared_ptr<Slider> create(Widget*, Position, Size, std::string, const std::vector<ParameterizedText>&);

            WidgetType getWidgetType() const override;

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

        private:
            Slider(Position, Size, std::string, const std::vector<ParameterizedText>&);

            void createOrUpdateWidget() override;
            bool requireRenderer() const override;

            bool onKeyPressEvent(InputDeviceKey) override;
            bool onKeyReleaseEvent(InputDeviceKey) override;
            bool onMouseMoveEvent(int, int) override;
            void onResetStateEvent() override;

            std::shared_ptr<Texture> loadTexture(const UdaChunk*, std::string_view) const;
            void updateSliderValue(int);
            void moveSliderCursor() const;

            //display information
            static constexpr float TEXT_SHIFT_LENGTH = 25.0f;
            const std::string skinName;
            const std::vector<ParameterizedText> values;
            unsigned int selectedIndex;

            //state
            enum TextBoxStates {
                DEFAULT,
                CURSOR_SELECTED,
            };
            TextBoxStates state;

            //visual
            std::shared_ptr<Text> currentValueText;
            std::shared_ptr<StaticBitmap> cursorImage;
            std::shared_ptr<Texture> texSliderLine;
    };

}
