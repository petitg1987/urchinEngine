#pragma once

#include <string>
#include <memory>
#include <vector>

#include "scene/ui/widget/Widget.h"
#include "scene/ui/widget/Position.h"
#include "scene/ui/widget/Size.h"
#include "scene/ui/widget/text/Text.h"
#include "scene/ui/widget/container/Container.h"
#include "scene/ui/widget/staticbitmap/StaticBitmap.h"
#include "scene/ui/EventListener.h"
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

        private:
            Sequence(Position, Size, std::string, const std::vector<ParameterizedText>&);

            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

            bool onKeyPressEvent(InputKey) override;
            bool triggerButton(Widget&) const;

            void moveProgressCursor() const;

            void setupLeftButtonListeners();
            void setupRightButtonListeners();

            static constexpr float PROGRESS_BAR_SHIFT_TOP_PERC = 0.15f;
            static constexpr float PROGRESS_BAR_SHIFT_LEFT_PERC = 0.10f;
            static constexpr float PROGRESS_BAR_HEIGHT_RATIO = 0.04f;

            //display information
            const std::string skinName;
            const std::vector<ParameterizedText> values;
            bool loopOnValuesEnabled;
            unsigned int selectedIndex;

            std::shared_ptr<EventListener> leftButtonEventListener;
            std::shared_ptr<EventListener> rightButtonEventListener;
            class ButtonSequenceEventListener final : public EventListener {
                public:
                    ButtonSequenceEventListener(Sequence*, bool);
                    bool onMouseLeftClickRelease(Widget*) override;

                private:
                    Sequence* sequence;
                    bool isLeftButton;
            };

            //visual
            std::shared_ptr<Container> leftButton;
            std::shared_ptr<Container> rightButton;
            std::vector<std::shared_ptr<Text>> valuesText;
            std::shared_ptr<StaticBitmap> progressLineImage;
            std::shared_ptr<StaticBitmap> progressCursorImage;
    };

}
