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

    class Slider : public Widget {
        public:
            static std::shared_ptr<Slider> create(Widget*, Position, Size, std::string, const std::vector<std::string>&);
            static std::shared_ptr<Slider> createTranslatable(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

        protected:
            void createOrUpdateWidget() override;
            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            void prepareWidgetRendering(float, unsigned int&) override;

        private:
            Slider(Position, Size, std::string, const std::vector<std::string>&, bool);

            std::shared_ptr<Texture> loadTexture(const UdaChunk*, const std::string&) const;
            void updateSliderValue(int);
            void moveSliderCursor();

            //display information
            const float TEXT_SHIFT_LENGTH;
            const std::string skinName;
            const std::vector<std::string> values;
            bool translatableValues;
            unsigned int selectedIndex;

            //state
            enum TextBoxStates {
                DEFAULT,
                CURSOR_SELECTED,
            } state;

            //visual
            std::shared_ptr<Text> currentValueText;
            std::shared_ptr<StaticBitmap> cursorImage;
            std::shared_ptr<Texture> texSliderLine;
            std::unique_ptr<GenericRenderer> sliderRenderer;
    };

}
