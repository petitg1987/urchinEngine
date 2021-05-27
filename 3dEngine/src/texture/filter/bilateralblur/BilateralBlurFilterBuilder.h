#ifndef URCHINENGINE_BILATERALBLURFILTERBUILDER_H
#define URCHINENGINE_BILATERALBLURFILTERBUILDER_H

#include "texture/filter/TextureFilterBuilder.h"

namespace urchin {

    class BilateralBlurFilterBuilder : public TextureFilterBuilder<BilateralBlurFilterBuilder> {
        public:
            enum BlurDirection {
                HORIZONTAL_BLUR,
                VERTICAL_BLUR
            };

            explicit BilateralBlurFilterBuilder(std::string, const std::shared_ptr<Texture>&);
            ~BilateralBlurFilterBuilder() override = default;

            BilateralBlurFilterBuilder* blurDirection(BlurDirection);

            BilateralBlurFilterBuilder* blurSize(unsigned int);
            unsigned int getBlurSize() const;

            BilateralBlurFilterBuilder* blurSharpness(float);
            float getBlurSharpness() const;

            BilateralBlurFilterBuilder* depthTexture(const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getDepthTexture() const;

            std::unique_ptr<TextureFilter> build() override;
            std::unique_ptr<BilateralBlurFilter> buildBilateralBlur();

        private:
            BlurDirection pBlurDirection;
            unsigned int pBlurSize;
            float pBlurSharpness;
            std::shared_ptr<Texture> pDepthTexture;
    };

}

#endif
