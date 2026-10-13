#pragma once

#include <string>
#include <vector>
#include <optional>

#include "scene/ui/widget/Widget.h"
#include "scene/ui/widget/Position.h"
#include "scene/ui/widget/Size.h"
#include "scene/ui/widget/text/Text.h"
#include "scene/ui/widget/staticbitmap/StaticBitmap.h"

namespace urchin {

    class Slider final : public Widget {
        public:
            static std::shared_ptr<Slider> create(Widget*, Position, Size, std::string, const std::vector<std::string>&);
            static std::shared_ptr<Slider> create(Widget*, Position, Size, std::string, const std::vector<ParameterizedText>&);

            WidgetType getWidgetType() const override;
            void prepareWidgetRendering(float) override;

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

        private:
            Slider(Position, Size, std::string, const std::vector<ParameterizedText>&);

            void createOrUpdateWidget() override;
            bool requireRenderer() const override;

            bool onKeyPressEvent(InputKey) override;
            bool onKeyReleaseEvent(InputKey) override;
            bool onMouseMoveEvent(int, int) override;
            void onResetStateEvent() override;
            void onGameControllerFocusUpdate() override;

            std::shared_ptr<Texture> loadTexture(const UdaChunk*, std::string_view) const;
            void updateSliderValue(int);
            void shiftSliderValue(int);
            void moveSliderCursor() const;
            void resetNavigationKeyPressed();

            static constexpr float TEXT_SHIFT_LENGTH = 25.0f;
            static constexpr float KEY_PRESSED_REPEAT_DELAY = 0.4f;
            static constexpr float KEY_PRESSED_REPEAT_INTERVAL_E2E = 4.0f;
            static constexpr float KEY_PRESSED_REPEAT_MAX_INTERVAL = 0.04f;

            //display information
            const std::string skinName;
            const std::vector<ParameterizedText> values;
            unsigned int selectedIndex;

            //state
            enum TextBoxStates {
                DEFAULT,
                CURSOR_SELECTED,
            };
            TextBoxStates state;
            std::optional<InputKey> navigationKeyPressed;
            float keyPressedRepeatCountdown;

            //visual
            std::shared_ptr<Text> currentValueText;
            std::shared_ptr<StaticBitmap> cursorImage;
            std::shared_ptr<Texture> texSliderLine;
    };

}
