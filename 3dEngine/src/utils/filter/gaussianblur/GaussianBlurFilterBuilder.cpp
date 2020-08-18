#include "GaussianBlurFilterBuilder.h"

namespace urchin
{

    GaussianBlurFilterBuilder::GaussianBlurFilterBuilder() :
        TextureFilterBuilder(),
        pBlurDirection(BlurDirection::HORIZONTAL_BLUR),
        pBlurSize(3) //3x3 blur
    {

    }

    GaussianBlurFilterBuilder *GaussianBlurFilterBuilder::blurDirection(BlurDirection blurDirection)
    {
        this->pBlurDirection = blurDirection;
        return this;
    }

    GaussianBlurFilterBuilder *GaussianBlurFilterBuilder::blurSize(unsigned int blurSize)
    {
        this->pBlurSize = blurSize;
        return this;
    }

    unsigned int GaussianBlurFilterBuilder::getBlurSize() const
    {
        return pBlurSize;
    }

    std::shared_ptr<TextureFilter> GaussianBlurFilterBuilder::build()
    {
        std::shared_ptr<TextureFilter> textureFilter;
        if(pBlurDirection==BlurDirection::HORIZONTAL_BLUR)
        {
            textureFilter = std::make_shared<GaussianBlurFilter>(this, GaussianBlurFilter::HORIZONTAL);
        }else if(pBlurDirection==BlurDirection::VERTICAL_BLUR)
        {
            textureFilter = std::make_shared<GaussianBlurFilter>(this, GaussianBlurFilter::VERTICAL);
        }else
        {
            throw std::invalid_argument("Unknown blur direction type: " + std::to_string(pBlurDirection));
        }

        textureFilter->initialize();

        return textureFilter;
    }

    std::shared_ptr<GaussianBlurFilter> GaussianBlurFilterBuilder::buildGaussianBlur()
    {
        return std::dynamic_pointer_cast<GaussianBlurFilter>(build());
    }

}
