#ifndef URCHINENGINE_WIDGETSIZE_H
#define URCHINENGINE_WIDGETSIZE_H

#include <scene/UI/widget/LengthType.h>
#include <scene/UI/widget/Length.h>

namespace urchin {

    class Size {
        public:
            Size(float, LengthType, float, LengthType);
            Size(float, float, LengthType);

            float getWidth() const;
            LengthType getWidthSizeType() const;

            float getHeight() const;
            LengthType getHeightSizeType() const;

        private:
            Length width;
            Length height;
    };

}

#endif
