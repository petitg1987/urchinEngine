#pragma once

#include <string>
#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>

namespace urchin {

    class Scrollbar {
        public:
            Scrollbar(Widget&, std::string);

            void initializeOrUpdate();

            bool onKeyPressEvent(unsigned int);
            bool onKeyReleaseEvent(unsigned int);
            bool onMouseMoveEvent(int, int);

        private:
            std::shared_ptr<Texture> loadTexture(const UdaChunk*, const std::string&) const;

            Widget& scrollableWidget;
            std::string skinName;

            std::shared_ptr<StaticBitmap> scrollbarCursor;
            std::shared_ptr<StaticBitmap> scrollbarLine;

            int mouseX, mouseY;
            enum ScrollbarStates {
                DEFAULT,
                CURSOR_SELECTED,
            } state;
    };

}