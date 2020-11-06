#ifndef URCHINENGINE_BILATERALBLURFILTERBUILDER_H
#define URCHINENGINE_BILATERALBLURFILTERBUILDER_H

#include "texturefilter/TextureFilterBuilder.h"

namespace urchin {

    class BilateralBlurFilterBuilder : public TextureFilterBuilder<BilateralBlurFilterBuilder> {
        public:
            enum BlurDirection
            {
                HORIZONTAL_BLUR,
                VERTICAL_BLUR
            };

            BilateralBlurFilterBuilder();
            ~BilateralBlurFilterBuilder() override = default;

            BilateralBlurFilterBuilder *blurDirection(BlurDirection);

            BilateralBlurFilterBuilder *blurSize(unsigned int);
            unsigned int getBlurSize() const;

            BilateralBlurFilterBuilder *blurSharpness(float);
            float getBlurSharpness() const;

            BilateralBlurFilterBuilder *depthTextureID(unsigned int);
            unsigned int getDepthTextureID() const;

            std::shared_ptr<TextureFilter> build() override;
            std::shared_ptr<BilateralBlurFilter> buildBilateralBlur();

        private:
            BlurDirection pBlurDirection;
            unsigned int pBlurSize;
            float pBlurSharpness;
            unsigned int pDepthTextureID;
    };

}

#endif
