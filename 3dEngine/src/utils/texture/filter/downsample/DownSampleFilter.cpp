#include "DownSampleFilter.h"
#include "utils/texture/filter/downsample/DownSampleFilterBuilder.h"

namespace urchin {

    DownSampleFilter::DownSampleFilter(const DownSampleFilterBuilder *textureFilterBuilder):
        TextureFilter(textureFilterBuilder) {

    }

    std::string DownSampleFilter::getShaderName() const {
        return "downSampleTex";
    }

    void DownSampleFilter::completeShaderTokens(std::map<std::string, std::string> &) const {
        //nothing to complete
    }

}
