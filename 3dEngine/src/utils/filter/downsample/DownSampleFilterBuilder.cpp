#include "DownSampleFilterBuilder.h"

namespace urchin
{

    DownSampleFilterBuilder::DownSampleFilterBuilder() :
        TextureFilterBuilder()
    {

    }

    std::shared_ptr<TextureFilter> DownSampleFilterBuilder::build()
    {
        std::shared_ptr<TextureFilter> textureFilter = std::make_shared<DownSampleFilter>(this);
        textureFilter->initialize();
        return textureFilter;
    }

    std::shared_ptr<DownSampleFilter> DownSampleFilterBuilder::buildDownSample()
    {
        return std::dynamic_pointer_cast<DownSampleFilter>(build());
    }

}
