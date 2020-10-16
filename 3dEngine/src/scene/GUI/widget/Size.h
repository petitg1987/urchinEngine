#ifndef URCHINENGINE_WIDGETSIZE_H
#define URCHINENGINE_WIDGETSIZE_H

namespace urchin {

    class Size {
        public:
            enum SizeType{
                PIXEL,
                PERCENTAGE
            };

            Size(float, SizeType, float, SizeType);
            Size(float, float, SizeType);

            float getWidth() const;
            SizeType getWidthSizeType() const;

            float getHeight() const;
            SizeType getHeightSizeType() const;

        private:
            float width, height;
            SizeType widthSizeType, heightSizeType;
    };

}

#endif
