#include "DownSampleFilterBuilder.h"

namespace urchin {

    DownSampleFilterBuilder::DownSampleFilterBuilder(std::string name, const std::shared_ptr<Texture>& sourceTexture) :
            TextureFilterBuilder(std::move(name), sourceTexture) {

    }

    std::unique_ptr<TextureFilter> DownSampleFilterBuilder::build() {
        std::unique_ptr<TextureFilter> textureFilter = std::make_unique<DownSampleFilter>(this);
        textureFilter->initialize();
        return textureFilter;
    }

    std::unique_ptr<DownSampleFilter> DownSampleFilterBuilder::buildDownSample() {
        std::unique_ptr<TextureFilter> downSample = build();
        return std::unique_ptr<DownSampleFilter>(dynamic_cast<DownSampleFilter*>(downSample.release()));
    }

}
