#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>

namespace urchin {

    /**
     * Container of widgets
     */
    class Container : public Widget {
        public:
            static std::shared_ptr<Container> newContainer(Widget*, Position);
            static std::shared_ptr<Container> newContainer(Widget*, Position, Size);

            void resetChildren();

            void enableScissor(bool);
            bool isScissorEnabled() const;

            void enableScrollbar(bool, const std::string& = "");

        protected:
            Container(Position, Size);

            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            std::shared_ptr<Texture> loadTexture(const UdaChunk*, const std::string&) const;

            bool scissorEnabled;

            bool scrollbarEnabled;
            std::string scrollbarSkinName;
            std::shared_ptr<StaticBitmap> scrollbarCursor;
            std::shared_ptr<StaticBitmap> scrollbarLine;
    };

}
