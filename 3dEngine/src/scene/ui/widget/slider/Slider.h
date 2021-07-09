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
            static Slider* newSlider(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

        protected:
            void createOrUpdateWidget() override;
            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            void prepareWidgetRendering(float) override;

        private:
            Slider(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            std::shared_ptr<Texture> loadTexture(const DataChunk*, const std::string&) const;
            void updateSliderValue(int);
            void moveSliderCursor();

            //display information
            const float TEXT_SHIFT_LENGTH;
            const std::string nameSkin;
            const std::vector<std::string> values;
            unsigned int selectedIndex;

            //state
            enum TextBoxStates {
                DEFAULT,
                CURSOR_SELECTED,
            } state;

            //visual
            Text* currentValueText;
            StaticBitmap *cursorImage;
            std::shared_ptr<Texture> texSliderLine;
            std::shared_ptr<GenericRenderer> sliderRenderer;
    };

}
