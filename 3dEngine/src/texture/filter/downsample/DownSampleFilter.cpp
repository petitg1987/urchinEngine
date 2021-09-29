#include <texture/filter/downsample/DownSampleFilter.h>
#include <texture/filter/downsample/DownSampleFilterBuilder.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    DownSampleFilter::DownSampleFilter(const DownSampleFilterBuilder* textureFilterBuilder):
        TextureFilter(textureFilterBuilder) {

    }

    std::string DownSampleFilter::getShaderName() const {
        if (getTextureFormat() == TextureFormat::RG_32_FLOAT && getTextureType() == TextureType::ARRAY) {
            return "texFilterDownSampleVec2Array";
        }
        throw std::runtime_error("Unimplemented down sample filter for: " + std::to_string((int)getTextureFormat()) + " - " + std::to_string((int)getTextureType()));
    }

    void DownSampleFilter::completeRenderer(const std::shared_ptr<GenericRendererBuilder>& textureRendererBuilder, const std::shared_ptr<TextureReader>& sourceTextureReader) {
        textureRendererBuilder->addUniformTextureReader(sourceTextureReader); //binding 1
    }

    std::unique_ptr<ShaderConstants> DownSampleFilter::buildShaderConstants() const {
        uint32_t textureLayer = getTextureLayer();
        std::vector<std::size_t> variablesSize = {sizeof(textureLayer)};
        return std::make_unique<ShaderConstants>(variablesSize, &textureLayer);
    }

}
