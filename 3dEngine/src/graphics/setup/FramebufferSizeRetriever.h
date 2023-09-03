#pragma once

namespace urchin {

    class FramebufferSizeRetriever {
        public:
            virtual ~FramebufferSizeRetriever() = default;

            /**
             * @param widthInPixel [out] return width in pixel (<> screen coordinate) or 0 if no drawable available.
             * @param heightInPixel [out] return height in pixel (<> screen coordinate) or 0 if no drawable available.
             */
            virtual void getFramebufferSizeInPixel(unsigned int& widthInPixel, unsigned int& heightInPixel) const = 0;
    };

}
