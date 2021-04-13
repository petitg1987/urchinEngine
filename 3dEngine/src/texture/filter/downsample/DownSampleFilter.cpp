#include "DownSampleFilter.h"
#include "texture/filter/downsample/DownSampleFilterBuilder.h"

namespace urchin {

    DownSampleFilter::DownSampleFilter(const DownSampleFilterBuilder* textureFilterBuilder):
        TextureFilter(textureFilterBuilder) {

    }

    std::string DownSampleFilter::getShaderName() const {
        if(getTextureFormat() == TextureFormat::RG_32_FLOAT && getTextureType() == TextureType::ARRAY) {
            return "texFilterDownSampleVec2Array";
        }
        throw std::runtime_error("Unimplemented down sample filter for: " + std::to_string(getTextureFormat()) + " - " + std::to_string(getTextureType()));
    }

    void DownSampleFilter::completeShaderTokens(std::map<std::string, std::string>&) const {
        //nothing to complete
    }

}
