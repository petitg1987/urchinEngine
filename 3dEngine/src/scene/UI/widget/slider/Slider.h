#pragma once

#include <string>
#include <vector>

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Size.h>
#include <scene/UI/widget/text/Text.h>
#include <scene/UI/widget/staticbitmap/StaticBitmap.h>
#include <scene/UI/EventListener.h>

namespace urchin {

    class Slider : public Widget {
        public:
            static Slider* newSlider(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

        protected:
            void createOrUpdateWidget() override;
            bool onKeyReleaseEvent(unsigned int) override;

            void prepareWidgetRendering(float) override;

        private:
            Slider(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            std::shared_ptr<Texture> loadTexture(const std::shared_ptr<XmlChunk>&, const std::string&) const;

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
