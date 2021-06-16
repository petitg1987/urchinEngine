#pragma once

#include <string>
#include <vector>

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Size.h>
#include <scene/UI/widget/text/Text.h>
#include <scene/UI/EventListener.h>

namespace urchin {

    class Slider : public Widget {
        public:
            static Slider* newSlider(Widget*, Position, Size, std::string, const std::vector<std::string>&);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            Slider(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            const std::vector<std::string> values;
            unsigned int selectedIndex;
    };

}
