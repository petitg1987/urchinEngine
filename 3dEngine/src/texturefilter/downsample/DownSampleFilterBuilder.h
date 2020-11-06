#ifndef URCHINENGINE_DOWNSAMPLEFILTERBUILDER_H
#define URCHINENGINE_DOWNSAMPLEFILTERBUILDER_H

#include "texturefilter/TextureFilterBuilder.h"

namespace urchin {

    class DownSampleFilterBuilder : public TextureFilterBuilder<DownSampleFilterBuilder> {
        public:
            DownSampleFilterBuilder();
            ~DownSampleFilterBuilder() override = default;

            std::shared_ptr<TextureFilter> build() override;
            std::shared_ptr<DownSampleFilter> buildDownSample();
    };

}

#endif
