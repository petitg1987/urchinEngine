#pragma once

#include <string>
#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>

namespace urchin {

    class Scrollbar {
        public:
            Scrollbar(Widget&, std::string);

            void initializeOrUpdate();
            void onScrollableWidgetsUpdated();

            bool onKeyPressEvent(unsigned int);
            bool onKeyReleaseEvent(unsigned int);
            bool onMouseMoveEvent(int, int);
            bool onScrollEvent(double);

            bool isScrollbarWidget(const Widget*) const;

            bool isScrollbarRequired() const;
            int getScrollShiftY() const;

        private:
            std::shared_ptr<Texture> loadTexture(const UdaChunk*, std::string_view) const;
            void updateScrollingPosition(int);
            void updateScrollingPosition();
            void updateCursorPosition() const;
            void computeShiftPositionY();
            std::vector<Widget*> getContentChildren() const;

            const float SCROLL_SPEED;

            Widget& scrollableWidget;
            std::string skinName;

            std::shared_ptr<StaticBitmap> scrollbarCursor;
            std::shared_ptr<StaticBitmap> scrollbarLine;

            float visibleHeight;
            float contentHeight;

            int mouseX, mouseY;
            enum ScrollbarStates {
                DEFAULT,
                CURSOR_SELECTED,
            } state;
            float scrollPercentage;
            int shiftPixelPositionY;
    };

}