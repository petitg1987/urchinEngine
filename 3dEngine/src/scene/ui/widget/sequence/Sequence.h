#pragma once

#include <string>
#include <memory>
#include <vector>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/widget/text/Text.h>
#include <scene/ui/EventListener.h>
namespace urchin {

    class Sequence final : public Widget {
        public:
            static std::shared_ptr<Sequence> create(Widget*, Position, Size, std::string, const std::vector<std::string>&);
            static std::shared_ptr<Sequence> create(Widget*, Position, Size, std::string, const std::vector<ParameterizedText>&);

            WidgetType getWidgetType() const override;

            void allowLoopOnValues(bool);
            bool isLoopOnValuesAllowed() const;

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

            void setLeftButtonEventListener(std::shared_ptr<EventListener>);
            void setRightButtonEventListener(std::shared_ptr<EventListener>);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            Sequence(Position, Size, std::string, const std::vector<ParameterizedText>&);

            void setupLeftButtonListeners();
            void setupRightButtonListeners();

            //display information
            const std::string skinName;
            const std::vector<ParameterizedText> values;
            bool loopOnValuesEnabled;
            unsigned int selectedIndex;

            std::shared_ptr<EventListener> leftButtonEventListener;
            std::shared_ptr<EventListener> rightButtonEventListener;
            class ButtonSequenceEventListener : public EventListener {
                public:
                    ButtonSequenceEventListener(Sequence*, bool);
                    bool onMouseLeftClickRelease(Widget*) override;

                private:
                    Sequence* sequence;
                    bool isLeftButton;
            };

            //visual
            std::shared_ptr<Text> leftButton;
            std::shared_ptr<Text> rightButton;
            std::vector<std::shared_ptr<Text>> valuesText;
    };

}
