#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>

namespace urchin {

    class Textarea : public Widget {
        public:
            static std::shared_ptr<Textarea> create(Widget*, Position, std::string);

            WidgetType getWidgetType() const override;

        protected:
            void createOrUpdateWidget() override;

        private:
            Textarea(Position, std::string);

            std::string skinName;
            std::string inputText;

            std::string text;
    };

}
