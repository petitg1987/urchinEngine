#include <graphics/render/GenericComputeBuilder.h>

namespace urchin {

    GenericComputeBuilder::GenericComputeBuilder(std::string name, RenderTarget& renderTarget, const Shader& shader) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader) {

    }

    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::create(std::string name, RenderTarget& renderTarget, const Shader& shader) {
        return std::shared_ptr<GenericComputeBuilder>(new GenericComputeBuilder(std::move(name), renderTarget, shader));
    }

    const std::string& GenericComputeBuilder::getName() const {
        return name;
    }

    RenderTarget& GenericComputeBuilder::getRenderTarget() const {
        return renderTarget;
    }

    const Shader& GenericComputeBuilder::getShader() const {
        return shader;
    }

    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::addUniformTextureReader(const std::shared_ptr<TextureReader>& textureReader) {
        uniformTextureReaders.push_back({textureReader});
        return shared_from_this();
    }

    const std::vector<std::vector<std::shared_ptr<TextureReader>>>& GenericComputeBuilder::getUniformTextureReaders() const {
        return uniformTextureReaders;
    }

    std::unique_ptr<GenericCompute> GenericComputeBuilder::build() {
        auto renderer = std::make_unique<GenericCompute>(*this);
        //TODO renderTarget.addRenderer(renderer.get());
        return renderer;
    }

}