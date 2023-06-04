#include <utility>

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

    /**
     * @param dataPtr Shader data pointer. Data must be memory aligned with "alignas(N)" expression using std140 alignment:
     *   - N=4 for scalar
     *   - N=8 for Vector2
     *   - N=16 for Point3, Point4, Vector3, Vector4, Matrix3 (not working ?), Matrix4
     *   - N=16 for embedded struct
     *   - N=16 for an array (important: the array elements are rounded up to 16 bytes. Therefore, an array of float (4 bytes) in C++ won't match an array of float in the shader.)
     */
    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::addUniformData(std::size_t dataSize, const void* dataPtr) {
        if (!uniformTextureReaders.empty()) {
            throw std::runtime_error("Adding uniform data after uniform texture is discouraged. Uniform binding ID start first with all data and then with all textures.");
        }
        uniformData.emplace_back(dataSize, dataPtr);
        return shared_from_this();
    }

    const std::vector<ShaderDataContainer>& GenericComputeBuilder::getUniformData() const {
        return uniformData;
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
        renderTarget.addProcessor(renderer.get());
        return renderer;
    }

}