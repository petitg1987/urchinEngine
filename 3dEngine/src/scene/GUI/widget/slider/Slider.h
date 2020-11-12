#ifndef URCHINENGINE_SLIDER_H
#define URCHINENGINE_SLIDER_H

#include <string>
#include <memory>
#include <vector>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/EventListener.h"
namespace urchin {

    class Slider : public Widget {
        public:
            Slider(Position, Size, const std::vector<std::string> &, const std::string &);
            ~Slider() override = default;

            void createOrUpdateWidget() override;

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

            void setLeftButtonEventListener(const std::shared_ptr<EventListener> &);
            void setRightButtonEventListener(const std::shared_ptr<EventListener> &);

            void display(const ShaderVar &, float) override;

        private:
            std::string buttonNameFont, valuesNameFont;
            std::string leftButtonString, rightButtonString;
            const std::vector<std::string> values;
            unsigned int selectedIndex;

            //visual
            Text *leftButton, *rightButton;
            std::vector<Text *> valuesText;

            std::shared_ptr<EventListener> leftButtonEventListener;
            std::shared_ptr<EventListener> rightButtonEventListener;

            float timeInClickingState;
            float timeSinceLastChange;

            class ButtonSliderEventListener : public EventListener {
                public:
                    ButtonSliderEventListener(Slider *, bool);
                    void onClickRelease(Widget *) override;

                private:
                    Slider *slider;
                    bool isLeftButton;
            };
    };

}

#endif
