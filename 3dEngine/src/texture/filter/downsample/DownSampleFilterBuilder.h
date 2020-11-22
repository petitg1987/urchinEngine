#ifndef URCHINENGINE_DOWNSAMPLEFILTERBUILDER_H
#define URCHINENGINE_DOWNSAMPLEFILTERBUILDER_H

#include "texture/filter/TextureFilterBuilder.h"

namespace urchin {

    class DownSampleFilterBuilder : public TextureFilterBuilder<DownSampleFilterBuilder> {
        public:
            DownSampleFilterBuilder();
            ~DownSampleFilterBuilder() override = default;

            std::unique_ptr<TextureFilter> build() override;
            std::unique_ptr<DownSampleFilter> buildDownSample();
    };

}

#endif
