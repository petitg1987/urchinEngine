#pragma once

#include <texture/filter/TextureFilterBuilder.h>

namespace urchin {

    class DownSampleFilterBuilder : public TextureFilterBuilder<DownSampleFilterBuilder> {
        public:
            explicit DownSampleFilterBuilder(bool, std::string, const std::shared_ptr<Texture>&);
            ~DownSampleFilterBuilder() override = default;

            std::unique_ptr<TextureFilter> build() override;
            std::unique_ptr<DownSampleFilter> buildDownSample();
    };

}
