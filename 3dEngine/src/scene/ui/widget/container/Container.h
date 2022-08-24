#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <scene/ui/scrollbar/Scrollbar.h>
#include <scene/ui/scrollbar/Scrollable.h>

namespace urchin {

    /**
     * Container of widgets
     */
    class Container final : public Widget, public Scrollable {
        public:
            static std::shared_ptr<Container> create(Widget*, Position, Size);
            static std::shared_ptr<Container> createScrollable(Widget*, Position, Size, std::string);

            void onResize() override;

            void notifyChildrenUpdated() const;

            WidgetType getWidgetType() const override;

            void addChild(const std::shared_ptr<Widget>&) override;
            void detachChild(Widget*) override;
            void detachChildren() override;
            void resetChildren();

            bool isScrollable() const;
            void updateScrollShiftY(int) const;
            int getScrollShiftY() const override;
            void onScrollableContentUpdated() const override;

        protected:
            Container(Position, Size, std::string);

            void createOrUpdateWidget() override;
            bool onKeyPressEvent(InputDeviceKey) override;
            bool onKeyReleaseEvent(InputDeviceKey) override;
            bool onMouseMoveEvent(int, int) override;
            bool onScrollEvent(double) override;

        private:
            std::unique_ptr<Scrollbar> scrollbar;
    };

}
