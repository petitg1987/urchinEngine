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

    class Sequence : public Widget {
        public:
            static std::shared_ptr<Sequence> newSequence(Widget*, Position, Size, std::string, const std::vector<std::string>&);
            static std::shared_ptr<Sequence> newTranslatableSequence(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            void allowLoopOnValues(bool);
            bool isLoopOnValuesAllowed() const;

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

            void setLeftButtonEventListener(std::unique_ptr<EventListener>);
            void setRightButtonEventListener(std::unique_ptr<EventListener>);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            Sequence(Position, Size, std::string, const std::vector<std::string>&, bool);

            //display information
            const std::string nameSkin;
            const std::vector<std::string> values;
            bool translatableValues;
            bool loopOnValuesEnabled;
            unsigned int selectedIndex;

            std::unique_ptr<EventListener> leftButtonEventListener;
            std::unique_ptr<EventListener> rightButtonEventListener;
            class ButtonSequenceEventListener : public EventListener {
                public:
                    ButtonSequenceEventListener(Sequence*, bool);
                    void onMouseLeftClickRelease(Widget*) override;

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
