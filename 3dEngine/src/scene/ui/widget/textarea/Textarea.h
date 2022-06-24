#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>

namespace urchin {

    class Textarea : public Widget {
        public:
            static std::shared_ptr<Textarea> create(Widget*, Position, Size, std::string);

            WidgetType getWidgetType() const override;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Textarea(Position, Size, std::string);

            void onResetStateEvent() override;

            //properties info
            std::string skinName;
            std::string inputText;

            //display information
            std::string text;

            //visual
            std::shared_ptr<Texture> texTextareaDefault;
            std::shared_ptr<Texture> texTextareaFocus;
            std::unique_ptr<GenericRenderer> textareaRenderer;
    };

}
