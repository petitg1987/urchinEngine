#pragma once

#include <string>

#include "scene/ui/widget/Widget.h"
#include "scene/ui/widget/staticbitmap/StaticBitmap.h"

namespace urchin {

    class Scrollbar {
        public:
            Scrollbar(Widget&, std::string);

            void initializeOrUpdate();
            void onScrollableWidgetsUpdated();

            bool onKeyPressEvent(InputDeviceKey);
            bool onKeyReleaseEvent(InputDeviceKey);
            bool onMouseMoveEvent(int, int);
            bool onScrollEvent(double);

            bool isScrollbarWidget(const Widget*) const;

            bool isScrollbarRequired() const;
            void updateScrollShiftY(int);
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

            int mouseX;
            int mouseY;
            enum ScrollbarStates {
                DEFAULT,
                CURSOR_SELECTED,
            };
            ScrollbarStates state;
            float scrollPercentage;
            int shiftPixelPositionY;
    };

}