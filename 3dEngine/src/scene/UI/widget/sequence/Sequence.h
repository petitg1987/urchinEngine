#pragma once

#include <string>
#include <memory>
#include <vector>

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Size.h>
#include <scene/UI/widget/text/Text.h>
#include <scene/UI/EventListener.h>
namespace urchin {

    class Sequence : public Widget {
        public:
            static Sequence* newSequence(Widget*, Position, Size, std::string, const std::vector<std::string>&);
            static Sequence* newTranslatableSequence(Widget*, Position, Size, std::string, const std::vector<std::string>&);

            void allowLoopOnValues(bool);

            unsigned int getSelectedIndex() const;
            void setSelectedIndex(unsigned int);

            void setLeftButtonEventListener(const std::shared_ptr<EventListener>&);
            void setRightButtonEventListener(const std::shared_ptr<EventListener>&);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            Sequence(Widget*, Position, Size, std::string, const std::vector<std::string>&, bool);

            //display information
            const std::string nameSkin;
            const std::vector<std::string> values;
            bool translatableValues;
            bool loopOnValuesEnabled;
            unsigned int selectedIndex;

            std::shared_ptr<EventListener> leftButtonEventListener;
            std::shared_ptr<EventListener> rightButtonEventListener;

            float timeInClickingState;
            float timeSinceLastChange;

            class ButtonSequenceEventListener : public EventListener {
                public:
                    ButtonSequenceEventListener(Sequence*, bool, bool);
                    void onMouseLeftClickRelease(Widget*) override;

                private:
                    Sequence* sequence;
                    bool isLeftButton;
                    bool loopOnValuesEnabled;
            };

            //visual
            Text* leftButton;
            Text* rightButton;
            std::vector<Text*> valuesText;
    };

}
