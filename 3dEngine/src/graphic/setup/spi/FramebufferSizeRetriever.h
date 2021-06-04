#pragma once

namespace urchin {

    class FramebufferSizeRetriever {
        public:
            virtual ~FramebufferSizeRetriever() = default;

            /**
             * @param widthInPixel [out] return width in pixel (<> screen coordinate)
             * @param heightInPixel [out] return height in pixel (<> screen coordinate)
             */
            virtual void getFramebufferSizeInPixel(int& widthInPixel, int& heightInPixel) const = 0;
    };

}
