#ifndef URCHINENGINE_STATICBITMAP_H
#define URCHINENGINE_STATICBITMAP_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "resources/image/Image.h"
#include "graphic/displayer/generic/GenericDisplayer.h"

namespace urchin {

    class StaticBitmap : public Widget {
        public:
            StaticBitmap(Position, Size, std::string );
            ~StaticBitmap() override;

            void createOrUpdateWidget() override;

            void display(int, float) override;

        private:
            //properties
            const std::string filename;

            //visual
            Image *tex;
            std::shared_ptr<GenericDisplayer> bitmapDisplayer;
    };

}

#endif
