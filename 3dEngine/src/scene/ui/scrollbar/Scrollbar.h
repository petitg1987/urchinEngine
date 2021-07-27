#pragma once

#include <string>
#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>

namespace urchin {

    class Scrollbar {
        public:
            Scrollbar(Widget&, const std::string&);

            void initializeOrUpdate();

            bool onKeyPressEvent(unsigned int);
            bool onKeyReleaseEvent(unsigned int);
            bool onMouseMoveEvent(int, int);

        private:
            std::shared_ptr<Texture> loadTexture(const UdaChunk*, const std::string&) const;

            Widget& scrollableWidget;
            const std::string& skinName;

            std::shared_ptr<StaticBitmap> scrollbarCursor;
            std::shared_ptr<StaticBitmap> scrollbarLine;
    };

}